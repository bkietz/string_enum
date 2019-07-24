#include <iostream>
#include <rekt/array.hpp>
#include <rekt/flat_multiset.hpp>
#include <rekt/matcher.hpp>
#include <rekt/trie.hpp>

namespace rekt
{

constexpr array<int, 4> ints({ 0, 1, 2, 3 });
static_assert(lexicographic_compare(ints, array<int, 4>({ 0, 1, 2, 3 })) == 0, "equality comparison");
static_assert(lexicographic_compare(ints, array<int, 1>({ -1 })) > 0, "greater than comparison");
static_assert(lexicographic_compare(ints, array<int, 1>({ 1 })) < 0, "less than comparison");
static_assert(lexicographic_compare(ints, array<int, 5>({ 0, 1, 2, 3, 4 })) < 0, "prefix compares less than prefixed");

static_assert(lexicographic_compare(ints.emplace(0, 4), array<int, 5>({ 4, 0, 1, 2, 3 })) == 0, "should emplace at the beginning");
static_assert(lexicographic_compare(ints.emplace(2, 4), array<int, 5>({ 0, 1, 4, 2, 3 })) == 0, "should emplace in the middle");
static_assert(lexicographic_compare(ints.emplace(4, 4), array<int, 5>({ 0, 1, 2, 3, 4 })) == 0, "should emplace at the end");
static_assert(lexicographic_compare(ints.emplace_back(4), array<int, 5>({ 0, 1, 2, 3, 4 })) == 0, "should emplace at the end");

constexpr bind_right_hand_side<decltype(less), int> less_than(int i)
{
  return bind_right_hand_side<decltype(less), int> { less, i };
};
static_assert(partition(ints.emplace(2, 4), less_than(3)).partition_point == 3, "partition an array");

static_assert(lexicographic_compare(sort(array<int, 3>({ 0, 1, 2 })), array<int, 3>({ 0, 1, 2 })) == 0, "3 element sort");
static_assert(lexicographic_compare(sort(array<int, 3>({ 1, 2, 0 })), array<int, 3>({ 0, 1, 2 })) == 0, "3 element sort");
static_assert(lexicographic_compare(sort(array<int, 3>({ 2, 0, 1 })), array<int, 3>({ 0, 1, 2 })) == 0, "3 element sort");
static_assert(lexicographic_compare(sort(array<int, 3>({ 2, 1, 0 })), array<int, 3>({ 0, 1, 2 })) == 0, "3 element sort");
static_assert(lexicographic_compare(sort(array<int, 3>({ 0, 2, 1 })), array<int, 3>({ 0, 1, 2 })) == 0, "3 element sort");
static_assert(lexicographic_compare(sort(array<int, 3>({ 1, 0, 2 })), array<int, 3>({ 0, 1, 2 })) == 0, "3 element sort");

static_assert(lexicographic_compare(sort(array<int, 4>({ 0, 3, 2, 1 })), array<int, 4>({ 0, 1, 2, 3 })) == 0, "4 element sort");
static_assert(lexicographic_compare(sort(array<int, 4>({ 0, 2, 3, 1 })), array<int, 4>({ 0, 1, 2, 3 })) == 0, "4 element sort");
static_assert(lexicographic_compare(sort(array<int, 4>({ 3, 0, 2, 1 })), array<int, 4>({ 0, 1, 2, 3 })) == 0, "4 element sort");
static_assert(lexicographic_compare(sort(array<int, 4>({ 3, 2, 0, 1 })), array<int, 4>({ 0, 1, 2, 3 })) == 0, "4 element sort");
static_assert(lexicographic_compare(sort(array<int, 4>({ 3, 2, 1, 0 })), array<int, 4>({ 0, 1, 2, 3 })) == 0, "4 element sort");
static_assert(lexicographic_compare(sort(array<int, 4>({ 3, 0, 1, 2 })), array<int, 4>({ 0, 1, 2, 3 })) == 0, "4 element sort");

static_assert(lexicographic_compare(sort(ints.emplace(0, 4)), array<int, 5>({ 0, 1, 2, 3, 4 })) == 0, "5 element sort");
static_assert(lexicographic_compare(sort(ints.emplace(1, 4)), array<int, 5>({ 0, 1, 2, 3, 4 })) == 0, "5 element sort");
static_assert(lexicographic_compare(sort(ints.emplace(2, 4)), array<int, 5>({ 0, 1, 2, 3, 4 })) == 0, "5 element sort");
static_assert(lexicographic_compare(sort(ints.emplace(3, 4)), array<int, 5>({ 0, 1, 2, 3, 4 })) == 0, "5 element sort");
static_assert(lexicographic_compare(sort(ints.emplace(4, 4)), array<int, 5>({ 0, 1, 2, 3, 4 })) == 0, "5 element sort");

constexpr flat_multiset<int, 5> ints_s(array<int, 5>({ 2, 3, 1, 4, 0 }));
static_assert(lexicographic_compare(ints_s.to_array(), array<int, 5>({ 0, 1, 2, 3, 4 })) == 0, "should be sorted at construction");
static_assert(lexicographic_compare(ints_s.emplace(2).to_array(), array<int, 6>({ 0, 1, 2, 2, 3, 4 })) == 0, "should be sorted after emplacement");
static_assert(lexicographic_compare(ints_s.emplace(2).pop_back().to_array(), array<int, 5>({ 0, 1, 2, 2, 3 })) == 0, "pop greatest element after emplacement");

template <int... I>
struct test_seq
{
  static constexpr auto size = sizeof...(I);
  static constexpr auto values = array<int, size>({ I... });
  static constexpr int value(std::size_t i)
  {
    return values[i];
  }
};

using a = test_seq<0, 0, 1, 2, 3, 3, 3, 3>;
static_assert(std::is_same<
                  group_by<a, group<0, a::size>>,
                  groups<group<0, 2>, group<2, 3>, group<3, 4>, group<4, a::size>>>::value,
              "");
static_assert(std::is_same<
                  group_by<a, group<1, a::size - 1>>,
                  groups<group<1, 2>, group<2, 3>, group<3, 4>, group<4, a::size - 1>>>::value,
              "");
static_assert(std::is_same<
                  group_by<a, group<3, a::size>>,
                  groups<group<3, 4>, group<4, a::size>>>::value,
              "");
static_assert(std::is_same<group_by<a, group<0, 4>>,
                           groups<group<0, 2>, group<2, 3>, group<3, 4>>>::value,
              "");

struct pass_int
{
  using return_type = int;
  template <int i>
  constexpr int operator()(std::integral_constant<int, i>) const { return i; }
  constexpr int operator()(default_tag) const { return 0; }
};
static_assert(make_switch<int, 1, 2, 3>::visit(1, pass_int {}) == 1, "1");
static_assert(make_switch<int, 1, 2, 3>::visit(2, pass_int {}) == 2, "2");
static_assert(make_switch<int, 1, 2, 3>::visit(3, pass_int {}) == 3, "3");
static_assert(make_switch<int, 1, 2, 3>::visit(5, pass_int {}) == 0, "5 -> default(0)");

namespace colors
{
constexpr string_enum values = R"(
  red
  yellow
  green
  cyan
  blue
  magenta
  light green
  light blue
  pink
)";

static_assert(values.size() == 9, "e.size()");
static_assert(values[0] == string_enum::string("red"), "red");
static_assert(values[2] == string_enum::string("green"), "green");
static_assert(values[4] == string_enum::string("blue"), "blue");
static_assert(values[8] == string_enum::string("pink"), "pink");

constexpr auto values_constant = string_enum::array_constant<values>::value;
static_assert(values_constant.size() == 9, "values_constant.size()");
static_assert(values_constant[0] == string_enum::string("blue"), "blue");
static_assert(values_constant[1] == string_enum::string("cyan"), "cyan");
static_assert(values_constant[7] == string_enum::string("red"), "red");
static_assert(values_constant[8] == string_enum::string("yellow"), "yellow");

constexpr auto index = trie<string_enum::array_constant<values>>::query;

constexpr std::size_t pass_or_throw(std::size_t index)
{
  return index != std::numeric_limits<std::size_t>::max()
      ? index
      : throw "invalid literal, string not in string_enum";
}

constexpr std::size_t operator""_color(char const *name, std::size_t)
{
  return pass_or_throw(index(name));
}

static_assert("blue"_color == 0, "blue");
static_assert("cyan"_color == 1, "cyan");
static_assert("light green"_color == 4, "light green");
static_assert("red"_color == 7, "red");

};

template <int I>
using i_c = std::integral_constant<int, I>;

struct enum_123_t
{
  template <typename Visitor>
  constexpr auto operator()(int i, Visitor &&v) const
      -> decltype(v(default_tag {}, i))
  {
    return i == 1
        ? v(i_c<1> {}, i)
        : i == 2
            ? v(i_c<2> {}, i)
            : i == 3
                ? v(i_c<3> {}, i)
                : v(default_tag {});
  }
};

constexpr enum_123_t enum_123 = {};

constexpr bool action_d(default_tag, int i)
{
  return i != 5;
}

constexpr bool action_1(i_c<1>, int)
{
  return true;
}

struct
{
  constexpr bool operator()(i_c<2>, int) const
  {
    return true;
  }
  constexpr bool operator()(i_c<3>, int) const
  {
    return true;
  }
} action_23;

constexpr auto arm_1 = type_arm<decltype(action_1) &, i_c<1>>(action_1);
constexpr auto arm_23 = type_arm<decltype(action_23), i_c<2>, i_c<3>>(action_23);
constexpr auto arm_d = type_arm<decltype(action_d) &, default_tag>(action_d);
constexpr auto w = when<i_c<1>>() = action_1;
constexpr auto one = enum_123(1, arm_1);
/*

constexpr bool dumb = matcher(
  when<i_c<1>>() = action_1,
  when<i_c<2>, i_c<3>>() = action_23,
  otherwise() = action_d
);
*/

} //namespace rekt

int main(int argc, char **)
{
  switch (argc)
  {
    using namespace rekt::colors;
  case "blue"_color:
    break;
  case "red"_color:
    break;
  case "green"_color:
    break;
  default:
    break;
  }
  return 0;
}
