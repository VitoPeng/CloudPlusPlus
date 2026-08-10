#include "../include/run.hpp"
#include "../include/CommandExecutor.hpp"
#include <string>

int run(std::string command, int argc, char *argv[]) {
  CommandExecutor executor;

  return executor.execute(command, argc, argv);
}