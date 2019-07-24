#pragma once

#include <limits>
#include <rekt/algorithm.hpp>
#include <rekt/array.hpp>
#include <type_traits>

namespace rekt
{

template <std::size_t B, std::size_t E>
struct group
{
  static constexpr auto begin = B;
  static constexpr auto end = E;
  static constexpr auto size = end - begin;
};

template <typename... Groups>
struct groups;

template <typename P, std::size_t I, typename W, typename G, typename... Gs>
struct group_by_impl;

template <typename Property, typename Within>
using group_by = typename group_by_impl<Property, Within::size, Within,
                                        group<Within::end - 1, Within::end>>::type;

template <typename P, typename W, typename G>
struct group_by_impl<P, 0, W, G>
{
  using type = groups<>;
};

template <typename P, typename W, typename G, typename... Gs>
struct group_by_impl<P, 1, W, G, Gs...>
{
  using type = groups<G, Gs...>;
};

template <bool Match, typename GBI>
struct make_new_group;

template <typename P, std::size_t I, typename W, typename G, typename... Gs>
struct make_new_group<true, group_by_impl<P, I, W, G, Gs...>>
    : group_by_impl<P, I - 1, W, group<G::begin - 1, G::end>, Gs...>
{
};

template <typename P, std::size_t I, typename W, typename G, typename... Gs>
struct make_new_group<false, group_by_impl<P, I, W, G, Gs...>>
    : group_by_impl<P, I - 1, W, group<G::begin - 1, G::begin>, G, Gs...>
{
};

template <typename P, std::size_t I, typename W, typename G, typename... Gs>
struct group_by_impl
    : make_new_group<P::value(G::begin - 1) == P::value(G::begin),
                     group_by_impl<P, I, W, G, Gs...>>
{
};

template <typename Condition, Condition... Cases>
struct make_switch;

struct default_tag
{
};

template <typename Visitor>
using return_type = typename std::decay<Visitor>::type::return_type;

template <typename Condition>
struct make_switch<Condition>
{
  template <typename Visitor, typename... Extra>
  static constexpr return_type<Visitor> visit(Condition, Visitor &&v, Extra &&... extra)
  {
    return v(default_tag {}, std::forward<Extra>(extra)...);
  }
};

template <typename Condition, Condition Case, Condition... Cases>
struct make_switch<Condition, Case, Cases...>
{
  template <typename Visitor, typename... Extra>
  static constexpr return_type<Visitor> visit(Condition c, Visitor &&v, Extra &&... extra)
  {
    return c == Case
        ? v(std::integral_constant<Condition, Case> {}, std::forward<Extra>(extra)...)
        : make_switch<Condition, Cases...>::visit(c, std::forward<Visitor>(v), std::forward<Extra>(extra)...);
  }
};

template <typename SortedStrings>
struct trie
{
  template <typename Visitor>
  static constexpr return_type<Visitor> visit(char const *str, Visitor &&v)
  {
    return subtrie<0, group_by<char_at<0>, group<0, SortedStrings::value.size()>>>::visit(str, std::forward<Visitor>(v));
  }

  struct query_visitor
  {
    using return_type = std::size_t;
    constexpr std::size_t operator()(std::size_t i, char const *) const
    {
      return i;
    }
    constexpr std::size_t operator()(default_tag, char const *) const
    {
      return std::numeric_limits<std::size_t>::max();
    }
  };

  static constexpr std::size_t query(char const *str)
  {
    return trie::visit(str, query_visitor {});
  }

  template <std::size_t I>
  struct char_at
  {
    static constexpr char value(std::size_t string_i)
    {
      return SortedStrings::value[string_i].size() == I
          ? '\0'
          : SortedStrings::value[string_i][I];
    }
  };

  struct subtrie_default
  {
    template <typename StringVisitor>
    constexpr return_type<StringVisitor> operator()(default_tag, StringVisitor &&v, char const *str) const
    {
      return v(default_tag {}, str);
    }
  };

  template <std::size_t I, char Condition, typename Group>
  struct subtrie_case
  {
    template <typename StringVisitor>
    constexpr return_type<StringVisitor> operator()(std::integral_constant<char, Condition>, StringVisitor &&v, char const *str) const
    {
      return subtrie<I + 1, group_by<char_at<I + 1>, Group>>::visit(str, std::forward<StringVisitor>(v));
    }
  };

  template <std::size_t I, typename Group>
  struct subtrie_case<I, '\0', Group>
  {
    template <typename StringVisitor>
    constexpr return_type<StringVisitor> operator()(std::integral_constant<char, '\0'>, StringVisitor &&v, char const *str) const
    {
      return v(std::integral_constant<std::size_t, Group::begin> {}, str);
    }
  };

  template <std::size_t I, typename Groups>
  struct subtrie;

  template <std::size_t I, typename... Groups>
  struct subtrie<I, groups<Groups...>>
  {
    template <typename StringVisitor>
    struct char_visitor : subtrie_default,
                          subtrie_case<I, char_at<I>::value(Groups::begin), Groups>...
    {
      using return_type = return_type<StringVisitor>;
    };
    template <typename StringVisitor>
    static constexpr return_type<StringVisitor> visit(char const *str, StringVisitor &&v)
    {
      using char_switch = make_switch<char, char_at<I>::value(Groups::begin)...>;
      return char_switch::visit(str[I], char_visitor<StringVisitor> {}, std::forward<StringVisitor>(v), str);
    }
  };
};

struct string_enum
{
  constexpr string_enum(char const *init)
      : init_(init)
      , init_end_(init + get_size(init))
      , size_(get_size(init))
  {
  }

  template <std::size_t N>
  constexpr string_enum(char const (&init)[N])
      : init_(init)
      , init_end_(N)
      , size_(get_size(init))
  {
  }

  static constexpr bool is_whitespace(char c)
  {
    return c == ' ' || c == '\n' || c == '\t' || c == '\r';
  }

  struct string
  {
    char const *begin;
    char const *end;

    constexpr std::size_t size() const
    {
      return end - begin;
    }

    constexpr char operator[](std::size_t i) const
    {
      return begin[i];
    }

    constexpr bool operator==(string const &o) const
    {
      return lexicographic_compare(*this, o) == 0;
    }

    constexpr bool operator<(string const &o) const
    {
      return lexicographic_compare(*this, o) < 0;
    }

    constexpr string(char const *before_begin)
        : begin { next_begin(before_begin) }
        , end { next_end(begin) }
    {
    }
  };

  constexpr string operator[](std::size_t i) const
  {
    return i == 0
        ? string(init_)
        : string_enum(next_end(next_begin(init_)))[i - 1];
  }

  constexpr std::size_t size() const
  {
    return size_;
  }

  template <string_enum const &e>
  struct array_constant;

private:
  std::size_t size_;
  char const *init_;
  char const *init_end_;

  constexpr static char const *next_begin(char const *before_begin)
  {
    return before_begin[0] == '\0'
        ? nullptr
        : is_whitespace(before_begin[0])
            ? next_begin(before_begin + 1)
            : before_begin;
  }

  constexpr static char const *next_end(char const *current_begin)
  {
    return current_begin[0] == '\0' || current_begin[0] == '\n' || current_begin[0] == '\r'
        ? current_begin
        : next_end(current_begin + 1);
  }

  constexpr static std::size_t get_size(char const *before_begin)
  {
    return next_begin(before_begin) == nullptr
        ? 0
        : get_size(next_end(next_begin(before_begin))) + 1;
  }
};

template <string_enum const &e>
struct string_enum::array_constant
{
  static constexpr auto value = sort(array<string, e.size()>(e));
};

} // namespace rekt
