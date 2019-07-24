#pragma once

#include <limits>
#include <rekt/algorithm.hpp>
#include <rekt/array.hpp>
#include <type_traits>

namespace rekt
{

// arms visit the unboxed value (const&) and the boxed value (auto&&)

// type_arms visit specified types
template <typename Action, typename... TypeConditions>
struct type_arm;

template <typename Action>
struct type_arm<Action>
{
  template <typename A>
  constexpr type_arm(A &&a)
      : action_(std::forward<A>(a))
  {
  }
  Action action_;
};

template <typename Action, typename Reference>
struct type_arm_impl
{
  constexpr Action const &action() const
  {
    return static_cast<type_arm<Action> const *>(this)->action_;
  }

  template <typename Boxed>
  constexpr auto operator()(Reference unboxed, Boxed &&b) const
      -> decltype(this->action()(static_cast<Reference>(unboxed), std::forward<Boxed>(b)))
  {
    return this->action()(static_cast<Reference>(unboxed), std::forward<Boxed>(b));
  }
};

template <typename Action, typename TypeCondition>
struct type_arm_refs
    : type_arm_impl<Action, TypeCondition &>,
      type_arm_impl<Action, TypeCondition &&>
{
  using type_arm_impl<Action, TypeCondition &>::operator();
  using type_arm_impl<Action, TypeCondition &&>::operator();
};

template <typename Action, typename TypeCondition>
struct type_arm_refs<Action, TypeCondition &> : type_arm_impl<Action, TypeCondition &>
{
  using type_arm_impl<Action, TypeCondition &>::operator();
};

template <typename Action, typename TypeCondition>
struct type_arm_refs<Action, TypeCondition &&> : type_arm_impl<Action, TypeCondition &&>
{
  using type_arm_impl<Action, TypeCondition &&>::operator();
};

template <typename Action, typename TypeCondition>
struct type_arm<Action, TypeCondition>
    : type_arm_refs<Action, TypeCondition>, type_arm<Action>
{
  using type_arm_refs<Action, TypeCondition>::operator();
  using type_arm<Action>::type_arm;
};

template <typename Action, typename T0, typename T1, typename... Ts>
struct type_arm<Action, T0, T1, Ts...>
    : type_arm<Action, T1, Ts...>,
      type_arm_refs<Action, T0>
{
  using type_arm<Action, T1, Ts...>::operator();
  using type_arm_refs<Action, T0>::operator();
  using type_arm<Action, T1, Ts...>::type_arm;
};

template <bool Condition>
using bool_constant = std::integral_constant<bool, Condition>;

template <bool... Conditions>
struct all_of_impl
{
  static std::true_type test(bool_constant<Conditions || true> const &...);
  static std::false_type test(...);
  static constexpr bool value = decltype(test(bool_constant<Conditions> {}...))::value;
};

template <>
struct all_of_impl<> : std::true_type
{
};

template <bool... Conditions>
using all_of = bool_constant<all_of_impl<Conditions...>::value>;

template <bool... Conditions>
using enable_if_all_of = typename std::enable_if<all_of<Conditions...>::value>::type;

template <typename Action, template <typename> class... Predicates>
struct template_arm
{
  static_assert(sizeof...(Predicates) > 0, "template_arm requires at least one predicate");

  Action action_;

  // FIXME I don't know what type the predicates should be applied to-
  // the below is "Unboxed if it is lvalue reference, otherwise remove_reference<Unboxed>"
  template <typename Unboxed, typename Boxed>
  constexpr auto operator()(Unboxed &&u, Boxed &&b,
                            enable_if_all_of<Predicates<Unboxed>::value...> * = nullptr) const
      -> decltype(action_(std::forward<Unboxed>(u), std::forward<Boxed>(b)))
  {
    return action_(std::forward<Unboxed>(u), std::forward<Boxed>(b));
  }
};

template <typename... Arms>
struct matcher;

template <typename A0, typename A1, typename... Arms>
struct matcher<A0, A1, Arms...> : A0, matcher<A1, Arms...>
{
  using A0::operator();
  using matcher<A1, Arms...>::operator();
};

template <typename Arm>
struct matcher<Arm> : Arm
{
  using Arm::operator();
};

template <typename T>
struct when
{
  template <typename Action>
  constexpr type_arm<Action, T> operator=(Action &&a) const
  {
    return type_arm<Action, T> { std::forward<Action>(a) };
  }
};

} // namespace rekt
