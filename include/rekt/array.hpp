#include <utility>

#pragma once

namespace rekt
{
template <std::size_t... I>
struct index_sequence {};

// TODO unroll this some
template <std::size_t Max, typename Seq = index_sequence<0>>
struct make_index_sequence;

template <std::size_t Max, std::size_t... I>
struct make_index_sequence<Max, index_sequence<Max, I...>>
  : index_sequence<I...>
{};

template <std::size_t Max, std::size_t Latest, std::size_t... I>
struct make_index_sequence<Max, index_sequence<Latest, I...>>
  : make_index_sequence<Max, index_sequence<sizeof...(I) + 1, I..., Latest>>
{};

template <typename T, std::size_t N>
class array
{
public:
  constexpr array() = delete;
  constexpr array(array const&) = default;
  array &operator=(array const&) = default;
  constexpr array(T const *ptr)
    : array{ ptr, make_index_sequence<N>{} }
  {}
  constexpr array(T const (&arr)[N])
    : array{ arr, make_index_sequence<N>{} }
  {}

  constexpr T const *data() const
  {
    return data_;
  }

  constexpr std::size_t size() const
  {
    return N;
  }

  constexpr T const *begin() const
  {
    return data_;
  }

  constexpr T const *end() const
  {
    return begin() + N;
  }

  constexpr T const &operator[](std::size_t i) const
  {
    return data_[i];
  }

private:
  template <std::size_t... I>
  constexpr array(T const *ptr, index_sequence<I...> const&)
    : data_{ ptr[I]... }
  {}

  T data_[N];
};

template <typename T>
class array_view
{
public:
  constexpr array_view() = delete;
  constexpr array_view(array_view const&) = default;
  array_view &operator=(array_view const&) = default;
  constexpr array_view(T const *ptr, std::size_t size)
    : data_{ ptr },
      size_{ size }
  {}
  template <std::size_t N>
  constexpr array_view(array<T, N> const &a)
    : data_{ a.data() },
      size_{ a.size() }
  {}
  template <std::size_t N>
  constexpr array_view(T const (&a)[N])
    : data_{ a },
      size_{ N }
  {}

  constexpr T const *data() const
  {
    return data_;
  }

  constexpr std::size_t size() const
  {
    return size_;
  }

  constexpr T const *begin() const
  {
    return data_;
  }

  constexpr T const *end() const
  {
    return begin() + size_;
  }

  constexpr T const &operator[](std::size_t i) const
  {
    return data_[i];
  }

private:
  T const *data_;
  std::size_t size_;
};

} // namespace rekt
