#include <format>

#include "./logging.hpp"
#include "./run.hpp"

int main(int argc, char* argv[]) {
  int numArguments = argc - 1;

  if (numArguments == 0) {
    write_error("Error: no commands specified. Pass in -h for help");

    return 1;
  }

  char* command = argv[1];

  int returnCode = run(command, argc, argv);

  if (returnCode == 0) {
    write_coloured("Program exited successfully\n", COLOUR_FG::GREEN);

    return 0;
  } else {
    write_error(
        std::format("Program exited with error code {:d}\n", returnCode));

    return returnCode;
  }
}