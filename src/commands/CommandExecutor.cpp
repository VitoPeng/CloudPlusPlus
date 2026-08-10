#include "./CommandExecutor.hpp"
#include "../logging.hpp"
#include <iostream>
#include <string>
#include <unordered_map>

CommandExecutor::CommandExecutor() {
  commands["help"] = [](int argc, char *argv[]) {
    std::cout << "No help page yet!\n";

    return 0;
  };

  alias("help", "-h");
}

void CommandExecutor::alias(std::string command, std::string alias) {
  commands[alias] = commands[command];
}

int CommandExecutor::execute(std::string command, int argc, char *argv[]) {
  auto executor = commands.find(command);

  if (executor != commands.end()) {
    return executor->second(argc, argv);
  } else {
    std::string errorMessage =
        std::string("Command \"").append(command).append("\" not found\n");

    writeColoured(errorMessage, COLOUR_FG::RED, COLOUR_BG::DEFAULT, &std::cerr);

    return 1;
  }
}