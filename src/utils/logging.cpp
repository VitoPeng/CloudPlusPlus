#include "../../include/logging.hpp"
#include <string>

// https://stackoverflow.com/a/11421489
std::ostream &operator<<(std::ostream &os, const COLOUR_FG &obj) {
  os << static_cast<std::underlying_type<COLOUR_FG>::type>(obj);
  return os;
}

std::ostream &operator<<(std::ostream &os, const COLOUR_BG &obj) {
  os << static_cast<std::underlying_type<COLOUR_BG>::type>(obj);
  return os;
}

void writeColoured(std::string text, COLOUR_FG fg, COLOUR_BG bg,
                   std::ostream *stream) {
  *stream << "\033[" << fg << ";" << bg << "m" << text << "\033[0m";
}