#include <rekt/flat_multiset.hpp>
#include <rekt/string_enum.hpp>

constexpr rekt::array<int, 4> ints({ 0, 1, 2, 3 });
static_assert(rekt::lexicographic_compare(ints, rekt::array<int, 4>({ 0, 1, 2, 3 })) == 0, "equality comparison");
static_assert(rekt::lexicographic_compare(ints, rekt::array<int, 1>({ -1 })) > 0, "greater than comparison");
static_assert(rekt::lexicographic_compare(ints, rekt::array<int, 1>({ 1 })) < 0, "less than comparison");
static_assert(rekt::lexicographic_compare(ints, rekt::array<int, 5>({ 0, 1, 2, 3, 4 })) < 0, "prefix compares less than prefixed");

static_assert(rekt::lexicographic_compare(ints.emplace(0, 4), rekt::array<int, 5>({ 4, 0, 1, 2, 3 })) == 0, "should emplace at the beginning");
static_assert(rekt::lexicographic_compare(ints.emplace(2, 4), rekt::array<int, 5>({ 0, 1, 4, 2, 3 })) == 0, "should emplace in the middle");
static_assert(rekt::lexicographic_compare(ints.emplace(4, 4), rekt::array<int, 5>({ 0, 1, 2, 3, 4 })) == 0, "should emplace at the end");
static_assert(rekt::lexicographic_compare(ints.emplace_back(4), rekt::array<int, 5>({ 0, 1, 2, 3, 4 })) == 0, "should emplace at the end");

constexpr rekt::bind_right_hand_side<decltype(rekt::less), int> less_than(int i)
{
  return rekt::bind_right_hand_side<decltype(rekt::less), int> { rekt::less, i };
};
static_assert(rekt::partition(ints.emplace(2, 4), less_than(3)).partition_point == 3, "partition an array");

static_assert(rekt::lexicographic_compare(rekt::sort(rekt::array<int, 3>({ 0, 1, 2 })), rekt::array<int, 3>({ 0, 1, 2 })) == 0, "3 element sort");
static_assert(rekt::lexicographic_compare(rekt::sort(rekt::array<int, 3>({ 1, 2, 0 })), rekt::array<int, 3>({ 0, 1, 2 })) == 0, "3 element sort");
static_assert(rekt::lexicographic_compare(rekt::sort(rekt::array<int, 3>({ 2, 0, 1 })), rekt::array<int, 3>({ 0, 1, 2 })) == 0, "3 element sort");
static_assert(rekt::lexicographic_compare(rekt::sort(rekt::array<int, 3>({ 2, 1, 0 })), rekt::array<int, 3>({ 0, 1, 2 })) == 0, "3 element sort");
static_assert(rekt::lexicographic_compare(rekt::sort(rekt::array<int, 3>({ 0, 2, 1 })), rekt::array<int, 3>({ 0, 1, 2 })) == 0, "3 element sort");
static_assert(rekt::lexicographic_compare(rekt::sort(rekt::array<int, 3>({ 1, 0, 2 })), rekt::array<int, 3>({ 0, 1, 2 })) == 0, "3 element sort");

static_assert(rekt::lexicographic_compare(rekt::sort(rekt::array<int, 4>({ 0, 3, 2, 1 })), rekt::array<int, 4>({ 0, 1, 2, 3 })) == 0, "4 element sort");
static_assert(rekt::lexicographic_compare(rekt::sort(rekt::array<int, 4>({ 0, 2, 3, 1 })), rekt::array<int, 4>({ 0, 1, 2, 3 })) == 0, "4 element sort");
static_assert(rekt::lexicographic_compare(rekt::sort(rekt::array<int, 4>({ 3, 0, 2, 1 })), rekt::array<int, 4>({ 0, 1, 2, 3 })) == 0, "4 element sort");
static_assert(rekt::lexicographic_compare(rekt::sort(rekt::array<int, 4>({ 3, 2, 0, 1 })), rekt::array<int, 4>({ 0, 1, 2, 3 })) == 0, "4 element sort");
static_assert(rekt::lexicographic_compare(rekt::sort(rekt::array<int, 4>({ 3, 2, 1, 0 })), rekt::array<int, 4>({ 0, 1, 2, 3 })) == 0, "4 element sort");
static_assert(rekt::lexicographic_compare(rekt::sort(rekt::array<int, 4>({ 3, 0, 1, 2 })), rekt::array<int, 4>({ 0, 1, 2, 3 })) == 0, "4 element sort");

static_assert(rekt::lexicographic_compare(rekt::sort(ints.emplace(0, 4)), rekt::array<int, 5>({ 0, 1, 2, 3, 4 })) == 0, "5 element sort");
static_assert(rekt::lexicographic_compare(rekt::sort(ints.emplace(1, 4)), rekt::array<int, 5>({ 0, 1, 2, 3, 4 })) == 0, "5 element sort");
static_assert(rekt::lexicographic_compare(rekt::sort(ints.emplace(2, 4)), rekt::array<int, 5>({ 0, 1, 2, 3, 4 })) == 0, "5 element sort");
static_assert(rekt::lexicographic_compare(rekt::sort(ints.emplace(3, 4)), rekt::array<int, 5>({ 0, 1, 2, 3, 4 })) == 0, "5 element sort");
static_assert(rekt::lexicographic_compare(rekt::sort(ints.emplace(4, 4)), rekt::array<int, 5>({ 0, 1, 2, 3, 4 })) == 0, "5 element sort");

constexpr rekt::flat_multiset<int, 5> ints_s(rekt::array<int, 5>({ 2, 3, 1, 4, 0 }));
static_assert(rekt::lexicographic_compare(ints_s.to_array(), rekt::array<int, 5>({ 0, 1, 2, 3, 4 })) == 0, "should be sorted at construction");
static_assert(rekt::lexicographic_compare(ints_s.emplace(2).to_array(), rekt::array<int, 6>({ 0, 1, 2, 2, 3, 4 })) == 0, "should be sorted after emplacement");
static_assert(rekt::lexicographic_compare(ints_s.emplace(2).pop_back().to_array(), rekt::array<int, 5>({ 0, 1, 2, 2, 3 })) == 0, "pop greatest element after emplacement");

int main()
{
  return 0;
}