#ifndef COMMAND_EXECUTOR
#define COMMAND_EXECUTOR

#include <functional>
#include <string>
#include <unordered_map>
class CommandExecutor {
public:
  CommandExecutor();

  int execute(std::string command, int argc, char *argv[]);

private:
  std::unordered_map<std::string, std::function<int(int argc, char *argv[])>>
      commands;

  void alias(std::string command, std::string alias);
};

#endif