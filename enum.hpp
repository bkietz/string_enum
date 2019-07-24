// clang-format off

using namespace std;

constexpr struct otherwise_t
{
  size_t const index = numeric_limits<size_t>::max();
} otherwise;

struct
{
  template <size_t I>
  constexpr size_t operator()(index_constant<I>) const
  {
    return J;
  }
  constexpr size_t operator()(otherwise_t) const
  {
    return otherwise.index;
  }
} return_index;

template <typename E>
concept Enumeration = requires(typename E::value_type v)
{
  // enumerations are sized
  { E::size } -> Same<size_t>;

  // enumerations allow visitation of index_constants or otherwise
  /*
  template <size_t I>
  requires E::visit_index(I, return_index) == I < E::size
    ? I
    : otherwise.index;
  */

  // enumerations also allow unboxing of value_types
  // values in the enumeration have an index
  { E::index(v) } -> Same<size_t>;

  // values can be visited as unboxed
  requires E::index(v) == E::visit(v, [](auto &&u)
  {
    // the unboxed types correspond to a known index or to 'otherwise'
    return return_index(E::template condition<decltype(u)&&> {});
  });
};

// clang-format on

template <typename... Constants>
struct constant_enumeration;

template <>
struct constant_enumeration<>
{
  using type = size_t;

  static constexpr size_t size = 0;

  static constexpr size_t index(type)
  {
    return otherwise.index;
  }

  template <size_t I>
  using constructor = decltype(otherwise);

  template <typename Visitor>
  static constexpr auto visit(type, Visitor &&v) -> decltype(v(otherwise))
  {
    return v(otherwise);
  }
};

template <typename Head, typename... Tail>
struct constant_enumeration<Head, Tail...>
{
  using type = size_t;

  static constexpr size_t size = sizeof...(Tail) + 1;

  static constexpr size_t index(type i)
  {
    return i < size ? i : otherwise.index;
  }

  template <size_t I>
  using constructor = typename conditional<
      I == 0,
      Head,
      constant_enumeration<Tail...>::constructor<I - 1>>::type;

  template <typename Visitor>
  static constexpr auto visit(type i, Visitor &&v) -> decltype(v(otherwise))
  {
    return i == 0
        ? v(Head {})
        : constant_enumeration<Tail...>::visit(i - 1, std::forward<Visitor>(v));
  }
};

template <typename Int, Int... I>
struct integer_enumeration;

template <typename Int>
struct integer_enumeration<Int>
{
  using value_type = Int;

  static constexpr size_t size = 0;

  template <typename UnboxedRef>
  using condition = decltype(otherwise);

  static constexpr size_t index(value_type)
  {
    return otherwise.index;
  }

  template <typename Visitor>
  static constexpr auto visit(value_type, Visitor &&v) -> decltype(v(otherwise))
  {
    return v(otherwise);
  }
};

template <typename Int, Int I0, Int... I>
struct integer_enumeration<Int, I0, I...>
{
  using value_type = Int;

  static constexpr size_t size = sizeof...(I) + 1;

  static constexpr size_t index(value_type i)
  {
    return i == I0
        ? 0
        : size == 1 // catch overflow
            ? otherwise.index
            : integer_enumeration<Int, I...>::index(i) + 1;
  }

  template <typename UnboxedRef>
  struct condition_impl
  {
    using type = decltype(otherwise);
  };

  template <value_type J>
  struct condition_impl<integral_constant<value_type, J> const &>
  {
    using type = index_constant<index(J)>;
  };

  template <typename UnboxedRef>
  using condition = typename condition_impl<UnboxedRef>::type;

  template <typename Visitor>
  static constexpr auto visit(value_type i, Visitor &&v) -> decltype(v(otherwise))
  {
    return i == I0
        ? v(integral_constant<Int, I0> {})
        : integer_enumeration<Int, I...>::visit(i, std::forward<Visitor>(v));
  }
};

template <typename Int, Int... I>
integer_enumeration<Int, I...> make_integer_enumeration(integer_sequence<Int, I...> const &)
{
  return {};
}

template <typename... T>
struct type_enumeration;

template <typename T0, typename... T>
struct type_enumeration<T0, T...>
{
  using value_type = size_t;

  static constexpr size_t size = sizeof...(T) + 1;

  //using indices = decltype(make_integer_enumeration(make_index_range<size>()));

  //using type_at = typename tuple_element<tuple<T...>>::type;

  static constexpr size_t index(value_type i)
  {
    return i < size ? i : otherwise.index;
  }

  template <typename UnboxedRef>
  struct condition_impl
  {
    using type = decltype(otherwise);
  };

  template <typename U>
  struct condition_impl<type_constant<U> const &>
      : conditional<is_same<U, type_constant<T0> const &>::value,
                    index_constant<0>,
                    index_constan<type_enumeration<T...>::template condition<U>::value + 1>>
  {
  };

  template <typename U>
  using condition = typename condition_impl<U>::type;

  template <typename Visitor>
  static constexpr auto visit(value_type i, Visitor &&v) -> decltype(v(otherwise))
  {
    return i == 0
        ? v(type_constant<T0> {})
        : integer_enumeration<T...>::visit(i - 1, std::forward<Visitor>(v));
  }
};

template <class Base, class... Derived>
struct class_enumeration;

template <class Base, class D0, class... D>
struct class_enumeration<Base, D0, D...>
{
  using value_type = Base const &;

  static constexpr size_t size = sizeof...(D) + 1;

  static constexpr size_t index(Base const &b)
  {
    return b.id();
  }
};

