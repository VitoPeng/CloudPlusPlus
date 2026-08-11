#include "./logging.hpp"
#include <format>
#include <string>

// https://stackoverflow.com/a/11421471
template <typename Enumeration>
auto as_integer(Enumeration const value) ->
    typename std::underlying_type<Enumeration>::type {
  return static_cast<typename std::underlying_type<Enumeration>::type>(value);
}

void write_coloured(std::string text, COLOUR_FG fg, COLOUR_BG bg,
                    std::ostream *stream) {
  *stream << std::format("\033[{:d};{:d}m{:s}\033[0m", as_integer(fg),
                         as_integer(bg), text);
}