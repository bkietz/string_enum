#pragma once

#include <rekt/array.hpp>
#include <utility>

namespace rekt
{

template <typename E, std::size_t N>
constexpr array<E, N> swap(array<E, N> const &a, std::size_t i, std::size_t j);

template <typename E, std::size_t N, std::size_t... I>
constexpr array<E, N> swap(array<E, N> const &a, std::size_t i, std::size_t j,
                           index_sequence<I...> const &);

// rassafrassa std::make_pair isn't constexpr
template <typename E, std::size_t N>
struct partition_result
{
  array<E, N> container;
  std::size_t partition_point;
};

template <typename E, std::size_t N, typename P>
constexpr partition_result<E, N> partition(array<E, N> const &a, P const &p);

template <typename E, std::size_t N, typename P>
constexpr partition_result<E, N> partition_range(array<E, N> const &a,
                                                 std::size_t b, std::size_t e,
                                                 P const &p);

template <typename I, typename P>
I find_if(I b, I e, P const &p);

template <typename E, std::size_t N, typename P>
constexpr array<E, N> sort_2(array<E, N> const &a, std::size_t b, P const &p);

template <typename E, std::size_t N, typename P>
constexpr array<E, N> sort_3(array<E, N> const &a, std::size_t b, P const &p);

template <typename E, std::size_t N, typename P>
constexpr array<E, N> sort_4_plus(partition_result<E, N> const &a_p,
                                  std::size_t b, std::size_t e, P const &p);

template <typename E, std::size_t N, typename P>
constexpr array<E, N> sort_4_plus(array<E, N> const &a, std::size_t b,
                                  std::size_t e, P const &p);

template <typename E, std::size_t N, typename P>
constexpr array<E, N> sort_range(array<E, N> const &a, std::size_t b,
                                 std::size_t e, P const &p);

template <typename E, std::size_t N, typename P>
constexpr array<E, N> sort(array<E, N> const &a, P const &p);

template <typename E, std::size_t N, typename P>
constexpr std::size_t upper_bound_range(array<E, N> const &a, std::size_t b,
                                        std::size_t e, E const &v, P const &p);

template <typename E, std::size_t N, typename P>
constexpr std::size_t upper_bound(array<E, N> const &a, E const &v, P const &p);

template <typename E, std::size_t N, typename P>
constexpr std::size_t lower_bound_range(array<E, N> const &a, std::size_t b,
                                        std::size_t e, E const &v, P const &p);

template <typename E, std::size_t N, typename P>
constexpr std::size_t lower_bound(array<E, N> const &a, E const &v, P const &p);

template <typename Left, typename Right>
constexpr int lexicographic_compare(Left const &l, Right const &r,
                                    std::size_t b = 0);

template <typename T, typename P>
constexpr T min(T const &l, T const &r, P const &p);

template <typename T>
constexpr T min(T const &l, T const &r);

// rassafrassa lambdas aren't constexpr
template <typename P, typename T>
class bind_right_hand_side
{
public:
  constexpr bind_right_hand_side(P const &p, T const &rhs)
      : p_ { p }
      , rhs_ { rhs }
  {
  }

  constexpr bool operator()(T const &l) const { return p_(l, rhs_); }

private:
  P p_;
  T rhs_;
};

constexpr struct
{
  template <typename T>
  constexpr bool operator()(T const &l, T const &r) const
  {
    return l < r;
  }
} less;

template <typename E, std::size_t N>
constexpr array<E, N> sort(array<E, N> const &a);

template <typename E, std::size_t N>
constexpr array<E, N> swap(array<E, N> const &a, std::size_t i, std::size_t j)
{
  return swap(a, i, j, make_index_sequence<N> {});
}

template <typename E, std::size_t N, std::size_t... I>
constexpr array<E, N> swap(array<E, N> const &a, std::size_t i, std::size_t j,
                           index_sequence<I...> const &)
{
  return array<E, N>({ a[I == i ? j : I == j ? i : I]... });
}

template <typename E, std::size_t N, typename P>
constexpr partition_result<E, N> partition(array<E, N> const &a, P const &p)
{
  return partition_range(a, 0, N, p);
}

template <typename E, std::size_t N, typename P>
constexpr partition_result<E, N> partition_range(array<E, N> const &a,
                                                 std::size_t b, std::size_t e,
                                                 P const &p)
{
  return b == e ? partition_result<E, N> { a, b }
                : p(a[b]) ? partition_range(a, b + 1, e, p)
                          : !p(a[e - 1]) ? partition_range(a, b, e - 1, p)
                                         : partition_range(swap(a, b, e - 1),
                                                           b + 1, e - 1, p);
}

template <typename E, std::size_t N, typename P>
constexpr array<E, N> sort_2(array<E, N> const &a, std::size_t b, P const &p)
{
  return p(a[b], a[b + 1]) ? a : swap(a, b, b + 1);
}

template <typename E, std::size_t N, typename P>
constexpr array<E, N> sort_3(array<E, N> const &a, std::size_t b, P const &p)
{
  return p(a[b], a[b + 1])
      ? (p(a[b + 1], a[b + 2]) ? a : sort_2(swap(a, b + 1, b + 2), b, p))
      : sort_2(swap(a, b, p(a[b + 1], a[b + 2]) ? b + 1 : b + 2), b + 1,
               p);
}

template <typename E, std::size_t N, typename P>
constexpr array<E, N> sort_4_plus(partition_result<E, N> const &a_p,
                                  std::size_t b, std::size_t e, P const &p)
{
  return sort_range(
      sort_range(swap(swap(a_p.container, 2, a_p.partition_point - 1), 1,
                      a_p.partition_point - 2),
                 b, a_p.partition_point - 2, p),
      a_p.partition_point - 1, e, p);
}

template <typename E, std::size_t N, typename P>
constexpr array<E, N> sort_4_plus(array<E, N> const &a, std::size_t b,
                                  std::size_t e, P const &p)
{
  return sort_4_plus(
      partition_range(a, b + 3, e, bind_right_hand_side<P, E> { p, a[1] }), b, e,
      p);
}

template <typename E, std::size_t N, typename P>
constexpr array<E, N> sort_range(array<E, N> const &a, std::size_t b,
                                 std::size_t e, P const &p)
{
  return e - b < 2
      ? a
      : e - b == 2 ? sort_2(a, b, p)
                   : e - b == 3 ? sort_3(a, b, p)
                                : sort_4_plus(sort_3(a, b, p), b, e, p);
}

template <typename E, std::size_t N, typename P>
constexpr array<E, N> sort(array<E, N> const &a, P const &p)
{
  return sort_range(a, 0, N, p);
}

template <typename E, std::size_t N>
constexpr array<E, N> sort(array<E, N> const &a)
{
  return sort(a, less);
}

template <typename T, typename P>
constexpr T min(T const &l, T const &r, P const &p)
{
  return p(l, r) ? l : r;
}

template <typename T>
constexpr T min(T const &l, T const &r)
{
  return l < r ? l : r;
}

template <typename Left, typename Right>
constexpr int lexicographic_compare(Left const &l, Right const &r,
                                    std::size_t b)
{
  return min(l.size(), r.size()) == b
      ? (l.size() == r.size()
             ? 0
             : l.size() < r.size()
                 ? -1
                 : 1) // one sequence is a prefix of the other or both
                      // sequences are identical
      : l[b] == r[b] ? lexicographic_compare(l, r, b + 1)
                     : l[b] < r[b] ? -1 : 1;
}

template <typename E, std::size_t N, typename P>
constexpr std::size_t upper_bound_range(array<E, N> const &a, std::size_t b,
                                        std::size_t e, E const &v, P const &p)
{
  return b == e ? b
                : p(v, a[(b + e) / 2])
          ? upper_bound_range(a, b, (b + e) / 2, v, p)
          : upper_bound_range(a, (b + e) / 2 + 1, e, v, p);
}

template <typename E, std::size_t N, typename P>
constexpr std::size_t upper_bound(array<E, N> const &a, E const &v,
                                  P const &p)
{
  return upper_bound_range(a, 0, N, v, p);
}

template <typename E, std::size_t N>
constexpr std::size_t upper_bound(array<E, N> const &a, E const &v)
{
  return upper_bound_range(a, 0, N, v, less);
}

template <typename E, std::size_t N, typename P>
constexpr std::size_t lower_bound_range(array<E, N> const &a, std::size_t b,
                                        std::size_t e, E const &v, P const &p)
{
  return b == e ? b
                : !p(a[(b + e) / 2], v)
          ? lower_bound_range(a, b, (b + e) / 2, v, p)
          : lower_bound_range(a, (b + e) / 2 + 1, e, v, p);
}

template <typename E, std::size_t N, typename P>
constexpr std::size_t lower_bound(array<E, N> const &a, E const &v,
                                  P const &p)
{
  return lower_bound_range(a, 0, N, v, p);
}

template <typename E, std::size_t N>
constexpr std::size_t lower_bound(array<E, N> const &a, E const &v)
{
  return lower_bound_range(a, 0, N, v, less);
}

template <typename Iter, typename Test>
bool test_at(std::size_t I, Iter begin, Iter end, Test const &test)
{
  return begin + I >= end ? false : test(*(begin + I));
}

template <typename P>
struct not_
{
  constexpr not_(P const &p)
      : p_(p)
  {
  }

  template <typename T>
  constexpr bool operator()(T &&t) const
  {
    return !p(std::forward<T>(t));
  }

  P p_;
};

template <typename I, typename P>
I find_if_not(I b, I e, P const &p)
{
  return find_if(b, e, not_<P>(p));
}

template <typename I, typename P>
I find_if(I b, I e, P const &p)
{
  return test_at(0, b, e, p)
      ? b + 0
      : test_at(1, b, e, p)
          ? b + 1
          : test_at(2, b, e, p)
              ? b + 2
              : test_at(3, b, e, p)
                  ? b + 3
                  : test_at(4, b, e, p)
                      ? b + 4
                      : test_at(5, b, e, p)
                          ? b + 5
                          : test_at(6, b, e, p)
                              ? b + 6
                              : test_at(7, b, e, p)
                                  ? b + 7
                                  : b + 7 >= e
                                      ? e
                                      : find_if(b + 8, e, p);
}

} // namespace rekt
