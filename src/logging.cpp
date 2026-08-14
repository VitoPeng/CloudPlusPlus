#include "./logging.hpp"

#include <format>
#include <iostream>
#include <string>

// https://stackoverflow.com/a/11421471
template <typename Enumeration>
auto as_integer(Enumeration const value) ->
    typename std::underlying_type<Enumeration>::type {
  return static_cast<typename std::underlying_type<Enumeration>::type>(value);
}

void write_coloured(std::string text,
                    COLOUR_FG fg,
                    COLOUR_BG bg,
                    std::ostream* stream) {
  *stream << std::format("\033[{:d};{:d}m{:s}\033[0m", as_integer(fg),
                         as_integer(bg), text);
}

void write_error(std::string text, bool new_line) {
  write_coloured(text.append(new_line ? "\n" : ""), COLOUR_FG::RED,
                 COLOUR_BG::DEFAULT, &std::cerr);
}

void write_warning(std::string text, bool new_line) {
  write_coloured(text.append(new_line ? "\n" : ""), COLOUR_FG::YELLOW);
}