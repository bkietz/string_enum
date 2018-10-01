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
    : strings_{ sort(array<string_view, Count>({ string_view{ static_cast<Char const *>(strings), N - 1 }... }), strless) }
  {}
  
  template <std::size_t N>
  constexpr std::size_t operator()(Char const (&str)[N]) const
  {
    return lookup({ static_cast<Char const *>(str), N - 1 });
  }

  std::size_t operator()(std::string const &str) const
  {
    return lookup({ str.c_str(), str.size() });
  }

private:
  static constexpr bool strless(string_view l, string_view r)
  {
    return lexicographic_compare(l, r) < 0;
  }

  constexpr std::size_t lookup(string_view str) const
  {
    return lookup(lower_bound(strings_, str, strless), str);
  }

  constexpr std::size_t lookup(std::size_t i, string_view str) const
  {
    return lexicographic_compare(strings_[i], str) == 0 ? i : Count;
  }

  array<string_view, Count> strings_;
};

template <typename Char, std::size_t... N>
constexpr basic_string_enum<Char, sizeof...(N)> make_string_enum(Char const (&...strings)[N])
{
  return basic_string_enum<Char, sizeof...(N)>{ strings... };
}

} // namespace rekt
