#include "./config.hpp"
#include "../logging.hpp"
#include "exceptions.h"
#include "query.h"
#include "toml.hpp"
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <mysql++.h>
#include <system_error>
#include <unordered_map>

const std::string CONFIG_DIR =
    std::string(std::getenv("HOME")).append(("/.config/cloud++/"));
const std::string CONFIG_FILE = "config.toml";
const std::string CONFIG_FILE_PATH =
    std::filesystem::path(CONFIG_DIR).concat(CONFIG_FILE).string();

std::string
settings_or_config_file(std::string setting_name, std::string config_file_param,
                        std::string default_value,
                        std::unordered_map<std::string, std::string> settings,
                        toml::table *config);
int try_create_database(Config *configuration);

int config(int argc, char **argv) {
  char **arguments = argv + 2;
  int num_arguments = argc - 2;

  std::set<std::string> flags;
  std::unordered_map<std::string, std::string> settings;

  for (int i = 0; i < num_arguments; i++) {
    std::string argument = arguments[i];

    if (argument.starts_with("--")) {
      if (i < num_arguments - 1) {
        if (settings.find(argument) != settings.end()) {
          write_warning(std::format("Warning: duplicate value for argument "
                                    "{}. Latest value will be used",
                                    argument));
        }

        std::string argument_value = arguments[++i];
        settings.insert({argument, argument_value});
      } else {
        write_warning(std::format("Missing value for argument {}", argument));
      }
    } else if (argument.starts_with("-")) {
      flags.insert(argument);
    }
  }

  if (settings.find("--directory") == settings.end()) {
    write_error("--directory option not provided");

    return 1;
  }

  if (!std::filesystem::exists(CONFIG_DIR)) {
    std::cout << CONFIG_DIR << " not found; creating directory\n";
    std::filesystem::create_directory(CONFIG_DIR);
  };

  if (!std::filesystem::exists(CONFIG_FILE_PATH)) {
    // create new empty file
    std::cout << CONFIG_FILE_PATH << " not found. Creating config file\n";
    std::ofstream config_file(CONFIG_FILE_PATH);
    config_file.close();
  }

  toml::table config_file = toml::parse_file(CONFIG_FILE_PATH);

  std::string user =
      settings_or_config_file("--username", "user", "", settings, &config_file);
  std::string password = settings_or_config_file("--password", "password", "",
                                                 settings, &config_file);
  std::string address = settings_or_config_file(
      "--address", "addresss", "localhost:3306", settings, &config_file);
  std::string directory = settings_or_config_file(
      "--directory", "directory", "./files", settings, &config_file);

  try {
    directory = std::filesystem::weakly_canonical(directory);
  } catch (std::filesystem::filesystem_error error) {
    directory = std::filesystem::weakly_canonical("./files");
  }

  Config configuration = {
      user,
      password,
      address,
      directory,
  };

  save_config(&configuration);

  std::error_code error_code;

  bool successful = std::filesystem::create_directories(directory, error_code);

  if (!successful && error_code.value() != 0) {
    write_error(std::format("Failed to create directory \"{}\". Error: {}",
                            directory, error_code.message()));

    return 1;
  }

  try {
    return try_create_database(&configuration);
  } catch (const mysqlpp::ConnectionFailed &error) {
    switch (error.errnum()) {
    case 1045: {
      write_error("Error: incorrect server credentials");

      return 1;
    }
    default: {
      write_error(std::format("Error: {}\n", error.what()));

      return 1;
    }
    }
  }
}

std::string
settings_or_config_file(std::string setting_name, std::string config_file_param,
                        std::string default_value,
                        std::unordered_map<std::string, std::string> settings,
                        toml::table *config) {
  std::string value = (*config)[config_file_param].value_or(default_value);
  auto settings_iterator = settings.find(setting_name);

  if (settings_iterator != settings.end()) {
    value = settings_iterator->second == "" ? default_value
                                            : settings_iterator->second;
  }

  return value;
}

void save_config(Config *configuration) {
  toml::table config_file = toml::parse_file(CONFIG_FILE_PATH);

  config_file.insert_or_assign("user", configuration->user);
  config_file.insert_or_assign("password", configuration->password);
  config_file.insert_or_assign("address", configuration->address);
  config_file.insert_or_assign("directory", configuration->directory);

  std::ofstream file(CONFIG_FILE_PATH);
  file << config_file;
  file.close();
}

Config get_config() {
  toml::table configuration = toml::parse_file(CONFIG_FILE_PATH);

  std::string user = configuration["user"].value_or("");
  std::string password = configuration["password"].value_or("");
  std::string address = configuration["address"].value_or("");
  std::string directory = configuration["directory"].value_or("./files");

  try {
    directory = std::filesystem::weakly_canonical(directory);
  } catch (std::filesystem::filesystem_error error) {
    directory = std::filesystem::weakly_canonical("./files");
  }

  return Config{
      user,
      password,
      address,
      directory,
  };
}

int try_create_database(Config *configuration) {
  mysqlpp::TCPConnection connection = mysqlpp::TCPConnection(
      configuration->address.c_str(), nullptr,
      configuration->user == "" ? nullptr : configuration->user.c_str(),
      configuration->password.c_str());

  if (!connection.connected()) {
    write_error(
        std::format("Error: connection failure: {}", connection.error()));

    return 1;
  }

  mysqlpp::Query create_database =
      connection.query("CREATE DATABASE IF NOT EXISTS `Cloud++`");
  create_database.execute();

  if (connection.error() != std::string("")) {
    write_error(
        std::format("Error when creating database: {}\n", connection.error()));

    return 1;
  }

  connection.select_db("Cloud++");

  mysqlpp::Query create_user_table = connection.query("\
CREATE TABLE IF NOT EXISTS `Users` (\
user_id BINARY(16) PRIMARY KEY,\
username VARCHAR(64) NOT NULL UNIQUE,\
password_hash BINARY(32) NOT NULL,\
password_salt BINARY(4) NOT NULL\
)");

  create_user_table.execute();

  mysqlpp::Query create_files_table = connection.query("\
CREATE TABLE IF NOT EXISTS `Files` (\
owner BINARY(16) NOT NULL,\
file_id BINARY(16) PRIMARY KEY,\
location VARCHAR(1024) NOT NULL,\
FOREIGN KEY (`owner`) \
REFERENCES `Users` (`user_id`)\
)");

  create_files_table.execute();

  try {
    mysqlpp::Query create_index_on_files = connection.query(
        "ALTER TABLE `Files` ADD INDEX `owner_index` (`owner`)");
    create_index_on_files.execute();
  } catch (const mysqlpp::BadQuery &error) {
    // ignore the error if it reports a duplicate index
    if (error.errnum() != 1061) {
      throw error;
    }
  }

  return 0;
}