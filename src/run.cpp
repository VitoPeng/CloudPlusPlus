#include "./run.hpp"
#include "./commands/CommandExecutor.hpp"
#include <string>

int run(std::string command, int argc, char *argv[]) {
  CommandExecutor executor;

  return executor.execute(command, argc, argv);
}