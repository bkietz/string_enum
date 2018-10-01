#pragma once

#include <rekt/array.hpp>
#include <rekt/algorithm.hpp>

namespace rekt
{

struct already_sorted_tag {};

template <typename T, std::size_t N, typename Cmp = decltype(less)>
class flat_set
{
public:
  constexpr flat_set() = delete;
  constexpr flat_set(flat_set const&) = default;
  flat_set &operator=(flat_set const&) = default;

  constexpr flat_set(array<T, N> const &arr, Cmp const &cmp = {})
    : flat_set{ already_sorted_tag(), sort(arr, cmp), cmp }
  {}

  constexpr flat_set(T const *ptr, Cmp const &cmp = {})
    : flat_set{ ptr, cmp }
  {}

  constexpr flat_set(T const (&arr)[N], Cmp const &cmp = {})
    : flat_set{ arr, cmp }
  {}

  constexpr flat_set(already_sorted_tag, array<T, N> const &arr, Cmp const &cmp = {})
    : data_{ arr },
      less_{ cmp }
  {}

  constexpr array<T, N> to_array() const
  {
    return data_;
  }

  constexpr T const &operator[](std::size_t i) const
  {
    return begin()[i];
  }

  constexpr std::size_t size() const
  {
    return N;
  }

  constexpr T const *begin() const
  {
    return data_.begin();
  }

  constexpr T const *end() const
  {
    return begin() + N;
  }

  template <typename... A>
  constexpr flat_set<T, N + 1, Cmp> emplace(A &&...a) const
  {
    return insert(T{ std::forward<A>(a)... }, make_index_sequence<N>{});
  }

private:
  template <std::size_t... I>
  constexpr flat_set<T, N + 1, Cmp> insert(T const &e, index_sequence<I...> const&) const
  {
    return flat_set<T, N + 1, Cmp>
    {
      already_sorted_tag(),
      data_.emplace(lower_bound(data_, e, less_), e),
      less_
    };
  }

  array<T, N> data_;
  Cmp less_;
};

} // namespace rekt
