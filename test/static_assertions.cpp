#include <rekt/string_enum.hpp>
#include <rekt/flat_set.hpp>

constexpr rekt::array<int, 4> ints({ 0, 1, 2, 3 });
static_assert(rekt::lexicographic_compare(ints, rekt::array<int, 4>({ 0, 1, 2, 3 })) == 0, "equality comparison");
static_assert(rekt::lexicographic_compare(ints, rekt::array<int, 1>({ -1 })) > 0, "greater than comparison");
static_assert(rekt::lexicographic_compare(ints, rekt::array<int, 1>({ 1 })) < 0, "less than comparison");
static_assert(rekt::lexicographic_compare(ints, rekt::array<int, 5>({ 0, 1, 2, 3, 4 })) < 0, "prefix compares less than prefixed");

constexpr rekt::array<int, 5> ints_2 = ints.emplace(2, 4);
static_assert(rekt::lexicographic_compare(ints.emplace(2, 4), rekt::array<int, 5>({ 0, 1, 4, 2, 3 })) == 0, "should emplace in the middle");

constexpr rekt::flat_set<int, 5> ints_s({ 2, 3, 1, 4, 0 });
constexpr int const &i0 = ints_s[0];
constexpr rekt::array<int, 5> ints_s_arr = ints_s.to_array();
//constexpr int i0 = ints_s_arr[0];
//constexpr int i1 = ints_s_arr[1];
//constexpr int i2 = ints_s_arr[2];
//constexpr int i3 = ints_s_arr[3];
//constexpr int i4 = ints_s_arr[4];
static_assert(rekt::lexicographic_compare(ints_s, rekt::array<int, 5>({ 0, 1, 2, 3, 4 })) == 0, "should be sorted at construction");
//constexpr rekt::flat_set<int, 6> ints_s2 = ints_s.emplace(2);
//static_assert(rekt::lexicographic_compare(ints_s2.to_array(), rekt::array<int, 6>({ 0, 1, 2, 2, 3, 4 })) == 0, "should be sorted after emplacement");



int main()
{
  return 0;
}