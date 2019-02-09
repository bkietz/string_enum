#pragma once

#include <utility>

namespace rekt
{
template <std::size_t... I>
struct index_sequence
{
};

template <std::size_t Max, typename Accumulated = index_sequence<>, typename ToAppend = index_sequence<0, 1, 2, 3>>
struct make_index_sequence;

template <std::size_t Max, std::size_t... I>
struct make_index_sequence<Max, index_sequence<I...>, index_sequence<Max, Max + 1, Max + 2, Max + 3>>
    : index_sequence<I...>
{
};

template <std::size_t Max, std::size_t... I>
struct make_index_sequence<Max, index_sequence<I...>, index_sequence<Max - 1, Max, Max + 1, Max + 2>>
    : index_sequence<I..., Max - 1>
{
};

template <std::size_t Max, std::size_t... I>
struct make_index_sequence<Max, index_sequence<I...>, index_sequence<Max - 2, Max - 1, Max, Max + 1>>
    : index_sequence<I..., Max - 2, Max - 1>
{
};

template <std::size_t Max, std::size_t... I>
struct make_index_sequence<Max, index_sequence<I...>, index_sequence<Max - 3, Max - 2, Max - 1, Max>>
    : index_sequence<I..., Max - 3, Max - 2, Max - 1>
{
};

template <std::size_t Max, std::size_t... Latest, std::size_t... I>
struct make_index_sequence<Max, index_sequence<I...>, index_sequence<Latest...>>
    : make_index_sequence<Max, index_sequence<I..., Latest...>, index_sequence<Latest + 4 ...>>
{
};

template <typename T, std::size_t N>
class array
{
public:
  constexpr array() = delete;
  constexpr array(array const &) = default;
  array &operator=(array const &) = default;
  constexpr array(T const *ptr)
      : array { ptr, make_index_sequence<N> {} }
  {
  }
  constexpr array(T const (&arr)[N])
      : array { arr, make_index_sequence<N> {} }
  {
  }

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

  template <typename... A>
  constexpr array<T, N + 1> emplace(std::size_t i, A &&... a) const
  {
    return insert(i, T { std::forward<A>(a)... }, make_index_sequence<N + 1> {});
  }

  template <typename... A>
  constexpr array<T, N + 1> emplace_back(A &&... a) const
  {
    return insert(size(), T { std::forward<A>(a)... }, make_index_sequence<N + 1> {});
  }

private:
  template <std::size_t... I>
  constexpr array(T const *ptr, index_sequence<I...> const &)
      : data_ { ptr[I]... }
  {
  }

  template <std::size_t... I>
  constexpr array<T, N + 1> insert(std::size_t i, T const &e, index_sequence<I...> const &) const
  {
    return array<T, N + 1>({ I == i ? e : data_[I - (I >= i)]... });
  }

  T data_[N];
};

template <typename T>
class array_view
{
public:
  constexpr array_view() = delete;
  constexpr array_view(array_view const &) = default;
  array_view &operator=(array_view const &) = default;
  constexpr array_view(T const *ptr, std::size_t size)
      : data_ { ptr }
      , size_ { size }
  {
  }
  template <std::size_t N>
  constexpr array_view(array<T, N> const &a)
      : data_ { a.data() }
      , size_ { a.size() }
  {
  }
  template <std::size_t N>
  constexpr array_view(T const (&a)[N])
      : data_ { a }
      , size_ { N }
  {
  }

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
