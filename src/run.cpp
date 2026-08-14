#include "./run.hpp"

#include <string>

#include "./commands/CommandExecutor.hpp"

int run(std::string command, int argc, char* argv[]) {
  CommandExecutor executor;

  return executor.execute(command, argc, argv);
}