#include "./config.hpp"
#include "../../libs/toml.hpp"
#include "../logging.hpp"
#include "query.h"
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <mysql++.h>
#include <unordered_map>

std::string
settings_or_config_file(std::string setting_name, std::string config_file_param,
                        std::string default_value,
                        std::unordered_map<std::string, std::string> settings,
                        toml::table *config);

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
          write_coloured(std::format("Warning: duplicate value for argument "
                                     "{}. Latest value will be used\n",
                                     argument),
                         COLOUR_FG::YELLOW);
        }

        std::string argument_value = arguments[++i];
        settings.insert({argument, argument_value});
      } else {
        write_coloured(std::format("Missing value for argument {}\n", argument),
                       COLOUR_FG::YELLOW);
      }
    } else if (argument.starts_with("-")) {
      flags.insert(argument);
    }
  }

  const std::string CONFIG_DIR =
      std::string(std::getenv("HOME")).append(("/.config/cloud++/"));
  const std::string CONFIG_FILE = "config.toml";
  const std::string CONFIG_FILE_PATH =
      std::filesystem::path(CONFIG_DIR).concat(CONFIG_FILE).string();

  if (!std::filesystem::exists(CONFIG_DIR)) {
    std::cout << CONFIG_DIR << " not found; creating directory\n";
    std::filesystem::create_directory(CONFIG_DIR);
  };

  if (!std::filesystem::exists(CONFIG_FILE_PATH)) {
    // create new empty file
    std::cout << CONFIG_FILE_PATH << " not found; creating config file\n";
    std::ofstream config_file(CONFIG_FILE_PATH);
    config_file.close();
  }

  toml::table configuration = toml::parse_file(CONFIG_FILE_PATH);

  std::string password = settings_or_config_file(
      "--password", "db-server-password", "", settings, &configuration);
  std::string address =
      settings_or_config_file("--address", "server-addresss", "localhost:3306",
                              settings, &configuration);
  std::string user = settings_or_config_file("--username", "user", "", settings,
                                             &configuration);

  std::ofstream file(CONFIG_FILE_PATH);
  file << configuration;
  file.close();

  try {
    mysqlpp::TCPConnection connection = mysqlpp::TCPConnection(
        address.c_str(), nullptr, user == "" ? nullptr : user.c_str(),
        password.c_str());

    if (!connection.connected()) {
      write_coloured(
          std::format("Error: connection failure: {}", connection.error()),
          COLOUR_FG::RED, COLOUR_BG::DEFAULT, &std::cerr);

      return 1;
    }

    mysqlpp::Query query =
        connection.query("CREATE DATABASE IF NOT EXISTS `Cloud++`");
    query.execute();

    if (connection.error() != std::string("")) {
      write_coloured(std::format("Error: {}\n", connection.error()),
                     COLOUR_FG::RED, COLOUR_BG::DEFAULT, &std::cerr);
    }
  } catch (const mysqlpp::ConnectionFailed &error) {
    switch (error.errnum()) {
    case 1045: {
      write_coloured("Error: incorrect server credentials\n", COLOUR_FG::RED,
                     COLOUR_BG::DEFAULT, &std::cerr);
    }
    default: {
      write_coloured(std::format("Error: {}\n", error.what()), COLOUR_FG::RED,
                     COLOUR_BG::DEFAULT, &std::cerr);
      return 1;
      break;
    }
    }
  }

  return 0;
}

std::string
settings_or_config_file(std::string setting_name, std::string config_file_param,
                        std::string default_value,
                        std::unordered_map<std::string, std::string> settings,
                        toml::table *config) {
  std::string value =
      (*config)[config_file_param].value_or<std::string>(default_value.c_str());
  auto settings_iterator = settings.find(setting_name);

  if (settings_iterator != settings.end()) {
    value = settings_iterator->second;
  }

  (*config).insert_or_assign(config_file_param, value);

  return value;
}