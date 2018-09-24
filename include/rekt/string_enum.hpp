#pragma once

#include <rekt/algorithm.hpp>
#include <rekt/array.hpp>
#include <string>

namespace rekt
{

template <typename Char, std::size_t Count>
class basic_string_enum
{
public:
  using string_view = array_view<Char>;

  template <std::size_t... N>
  constexpr basic_string_enum(Char const (&...strings)[N])
    : strings_{ sort(array<string_view, Count>{ string_view{ static_cast<Char const *>(strings), N - 1 }... }, strless) }
  {}
  
  template <std::size_t N>
  constexpr int operator()(Char const (&str)[N]) const
  {
    return lookup({ static_cast<Char const *>(str), N - 1 }, 0, Count);
  }

  int operator()(std::string const &str) const
  {
    return lookup({ str.c_str(), str.size() }, 0, Count);
  }

private:
  static constexpr bool strless(string_view l, string_view r)
  {
    return lexicographic_compare(l, r) < 0;
  }

  static constexpr bool streq(string_view l, string_view r)
  {
    return lexicographic_compare(l, r) == 0;
  }

  constexpr int lookup(string_view str, std::size_t b, std::size_t e) const
  {
    return e - b == 1
      ? (streq(str, strings_[b]) ? b : -1)
      : strless(str, strings_[(b + e)/2])
        ? lookup(str, b, (b + e)/2)
        : lookup(str, (b + e)/2, e);
  }

  array<string_view, Count> strings_;
};

template <typename Char, std::size_t... N>
constexpr basic_string_enum<Char, sizeof...(N)> make_string_enum(Char const (&...strings)[N])
{
  return basic_string_enum<Char, sizeof...(N)>{ strings... };
}

} // namespace rekt