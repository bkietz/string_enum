#pragma once

#include <rekt/array.hpp>
#include <utility>

namespace rekt
{

template <typename E, std::size_t N>
constexpr array<E, N> swap(array<E, N> const &a, std::size_t i, std::size_t j);

template <typename E, std::size_t N, std::size_t... I>
constexpr array<E, N> swap(array<E, N> const &a, std::size_t i, std::size_t j, index_sequence<I...> const&);

template <typename E, std::size_t N, typename P>
constexpr std::pair<array<E, N>, std::size_t> partition(array<E, N> const &a, P const &p);

template <typename E, std::size_t N, typename P>
constexpr std::pair<array<E, N>, std::size_t>
partition_range(array<E, N> const &a, std::size_t b, std::size_t e, P const &p);

template <typename E, std::size_t N, typename P>
constexpr array<E, N> sort_2(array<E, N> const &a, std::size_t b, P const &p);

template <typename E, std::size_t N, typename P>
constexpr array<E, N> sort_3(array<E, N> const &a, std::size_t b, P const &p);

template <typename E, std::size_t N, typename P>
constexpr array<E, N> sort_4_plus(std::pair<array<E, N>, std::size_t> const &a_p, std::size_t b, std::size_t e, P const &p);

template <typename E, std::size_t N, typename P>
constexpr array<E, N> sort_4_plus(array<E, N> const &a, std::size_t b, std::size_t e, P const &p);

template <typename E, std::size_t N, typename P>
constexpr array<E, N> sort_range(array<E, N> const &a, std::size_t b, std::size_t e, P const &p);

template <typename E, std::size_t N, typename P>
constexpr array<E, N> sort(array<E, N> const &a, P const &p);

template <typename Left, typename Right>
constexpr int lexicographic_compare(Left const &l, Right const &r, std::size_t b = 0);

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
    : p_{ p },
      rhs_{ rhs }
  {}

  constexpr bool operator()(T const &l) const
  {
    return p_(l, rhs_);
  }

private:
  P const &p_;
  T const &rhs_;
};

constexpr struct
{
  template <typename T>
  constexpr bool operator()(T const &l, T const &r) const
  {
    return l < r;
  }
}
less;

template <typename E, std::size_t N>
constexpr array<E, N> sort(array<E, N> const &a);

template <typename E, std::size_t N>
constexpr array<E, N> swap(array<E, N> const &a, std::size_t i, std::size_t j)
{
  return swap(a, i, j, make_index_sequence<N>{});
}

template <typename E, std::size_t N, std::size_t... I>
constexpr array<E, N> swap(array<E, N> const &a, std::size_t i, std::size_t j, index_sequence<I...> const&)
{
  return { a[I == i ? j : I == j ? i : I]... };
}

template <typename E, std::size_t N, typename P>
constexpr std::pair<array<E, N>, std::size_t> partition(array<E, N> const &a, P const &p)
{
  return partition_range(a, 0, N, p);
}

template <typename E, std::size_t N, typename P>
constexpr std::pair<array<E, N>, std::size_t> partition_range(array<E, N> const &a, std::size_t b, std::size_t e, P const &p)
{
  return b == e
    ? std::make_pair(a, b)
    : p(a[b])
      ? partition_range(a, b + 1, e, p)
      : !p(a[e - 1])
        ? partition_range(a, b, e - 1, p)
        : partition_range(swap(a, b, e - 1), b + 1, e - 1, p);
}

template <typename E, std::size_t N, typename P>
constexpr array<E, N> sort_2(array<E, N> const &a, std::size_t b, P const &p)
{
  return p(a[b], a[b + 1])
    ? a
    : swap(a, b, b + 1);
}

template <typename E, std::size_t N, typename P>
constexpr array<E, N> sort_3(array<E, N> const &a, std::size_t b, P const &p)
{
  return p(a[b], a[b + 1])
    ? (p(a[b + 1], a[b + 2]) ? a : sort_2(swap(a, b + 1, b + 2), b, p))
    : sort_2(swap(a, b, p(a[b + 1], a[b + 2]) ? b + 1 : b + 2), b + 1, p);
}

template <typename E, std::size_t N, typename P>
constexpr array<E, N> sort_4_plus(std::pair<array<E, N>, std::size_t> const &a_p, std::size_t b, std::size_t e, P const &p)
{
  return sort_range(sort_range(swap(swap(a_p.first, 2, a_p.second - 1), 1, a_p.second - 2), b, a_p.second - 2, p), a_p.second - 2, e, p);
}

template <typename E, std::size_t N, typename P>
constexpr array<E, N> sort_4_plus(array<E, N> const &a, std::size_t b, std::size_t e, P const &p)
{
  return sort_4_plus(partition_range(a, b + 3, e, bind_right_hand_side<P, E>{ p, a[1] }), b, e, p);
}

template <typename E, std::size_t N, typename P>
constexpr array<E, N> sort_range(array<E, N> const &a, std::size_t b, std::size_t e, P const &p)
{
  return e - b < 2
    ? a
    : e - b == 2
      ? sort_2(a, b, p)
      : e - b == 3
        ? sort_3(a, b, p)
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
constexpr int lexicographic_compare(Left const &l, Right const &r, std::size_t b)
{
  return min(l.size(), r.size()) == b
    ? int(l.size()) - int(r.size()) // one sequence is a prefix of the other or both sequences are identical
    : l[b] == r[b]
      ? lexicographic_compare(l, r, b + 1)
      : l[b] < r[b]
        ? -1
        : 1;
}

} // namespace rekt