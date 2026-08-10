#include "./logging.hpp"
#include "./run.hpp"
#include <format>
#include <iostream>

int main(int argc, char *argv[]) {
  int numArguments = argc - 1;

  if (numArguments == 0) {
    writeColoured("Error: no commands specified. Pass in -h for help\n",
                  COLOUR_FG::RED, COLOUR_BG::DEFAULT, &std::cerr);

    return 1;
  }

  char *command = argv[1];

  int returnCode = run(command, argc, argv);

  if (returnCode == 0) {
    writeColoured("Program exited successfully\n", COLOUR_FG::GREEN);
    return 0;
  } else {
    writeColoured(
        std::format("Program exited with error code {:d}\n", returnCode),
        COLOUR_FG::RED, COLOUR_BG::DEFAULT, &std::cerr);
  }
}