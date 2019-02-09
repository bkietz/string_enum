#pragma once

#include <rekt/algorithm.hpp>
#include <rekt/array.hpp>

namespace rekt
{

struct already_sorted_tag
{
};

template <typename T, std::size_t N, typename Cmp = decltype(less)>
class flat_multiset
{
public:
  constexpr flat_multiset() = delete;
  constexpr flat_multiset(flat_multiset const &) = default;
  flat_multiset &operator=(flat_multiset const &) = default;

  constexpr flat_multiset(array<T, N> const &arr, Cmp const &cmp = {})
      : flat_multiset { already_sorted_tag(), sort(arr, cmp), cmp }
  {
  }

  constexpr flat_multiset(T const *ptr, Cmp const &cmp = {})
      : flat_multiset { array<T, N> { ptr }, cmp }
  {
  }

  constexpr flat_multiset(T const (&arr)[N], Cmp const &cmp = {})
      : flat_multiset { array<T, N> { static_cast<T const *>(arr) }, cmp }
  {
  }

  constexpr flat_multiset(already_sorted_tag, array<T, N> const &arr, Cmp const &cmp = {})
      : data_ { arr }
      , less_ { cmp }
  {
  }

  constexpr array<T, N> to_array() const
  {
    return data_;
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
  constexpr flat_multiset<T, N + 1, Cmp> emplace(A &&... a) const
  {
    return insert(T { std::forward<A>(a)... }, make_index_sequence<N> {});
  }

  constexpr T const &back() const
  {
    return data_[N - 1];
  }

  constexpr flat_multiset<T, N - 1, Cmp> pop_back() const
  {
    return flat_multiset<T, N - 1, Cmp> {
      already_sorted_tag(),
      array<T, N - 1> { data_.begin() },
      less_
    };
  }

private:
  template <std::size_t... I>
  constexpr flat_multiset<T, N + 1, Cmp> insert(T const &e, index_sequence<I...> const &) const
  {
    return flat_multiset<T, N + 1, Cmp> {
      already_sorted_tag(),
      data_.emplace(lower_bound(data_, e, less_), e),
      less_
    };
  }

  array<T, N> data_;
  Cmp less_;
};

} // namespace rekt
