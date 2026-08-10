#include "../include/logging.hpp"
#include <iostream>

int main(int argc, char *argv[]) {
  int numArguments = argc - 1;

  if (numArguments == 0) {
    writeColoured("Error: no commands specified. Pass in -h for help\n",
                  COLOUR_FG::RED, COLOUR_BG::DEFAULT, &std::cerr);
  }

  return 0;
}