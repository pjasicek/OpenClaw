/*
 * Copyright 2003 - 2016, The libsigc++ Development Team
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifndef SIGC_ADAPTORS_BIND_H
#define SIGC_ADAPTORS_BIND_H

#include <sigc++/adaptors/adapts.h>
#include <sigc++/adaptors/bound_argument.h>
#include <sigc++/adaptors/tuple_visitor_visit_each.h>
#include <sigc++/tuple-utils/tuple_for_each.h>
#include <sigc++/tuple-utils/tuple_start.h>
#include <sigc++/tuple-utils/tuple_end.h>
#include <sigc++/tuple-utils/tuple_transform_each.h>


namespace sigc
{

/** @defgroup bind bind(), bind_return()
 * sigc::bind() alters an arbitrary functor by fixing arguments to certain values.
 * Up to 7 arguments can be bound at a time.
 * For single argument binding, overloads of sigc::bind() are provided that let you
 * specify the zero-based position of the argument to fix with the first template parameter.
 * (A value of @p -1 fixes the last argument so sigc::bind<-1>() gives the same result as
 * sigc::bind().)
 * The types of the arguments can optionally be specified if not deduced.
 *
 * @par Examples:
 * @code
 * void foo(int, int, int);
 * // single argument binding ...
 * sigc::bind(&foo,1)(2,3);     //fixes the last (third) argument and calls foo(2,3,1)
 * sigc::bind<-1>(&foo,1)(2,3); //same as bind(&foo,1)(2,3) (calls foo(2,3,1))
 * sigc::bind<0>(&foo,1)(2,3);  //fixes the first argument and calls foo(1,2,3)
 * sigc::bind<1>(&foo,1)(2,3);  //fixes the second argument and calls foo(2,1,3)
 * sigc::bind<2>(&foo,1)(2,3);  //fixes the third argument and calls foo(2,3,1)
 * // multi argument binding ...
 * sigc::bind(&foo,1,2)(3);     //fixes the last two arguments and calls foo(3,1,2)
 * sigc::bind(&foo,1,2,3)();    //fixes all three arguments and calls foo(1,2,3)
 * @endcode
 *
 * The functor sigc::bind() returns can be passed into
 * sigc::signal::connect() directly.
 *
 * @par Example:
 * @code
 * sigc::signal<void()> some_signal;
 * void foo(int);
 * some_signal.connect(sigc::bind(&foo,1));
 * @endcode
 *
 * sigc::bind_return() alters an arbitrary functor by
 * fixing its return value to a certain value.
 *
 * @par Example:
 * @code
 * void foo();
 * std::cout << sigc::bind_return(&foo, 5)(); // calls foo() and returns 5
 * @endcode
 *
 * You can bind references to functors by passing the objects through
 * the std::ref() helper function.
 *
 * @par Example:
 * @code
 * int some_int;
 * sigc::signal<void()> some_signal;
 * void foo(int&);
 * some_signal.connect(sigc::bind(&foo, std::ref(some_int)));
 * @endcode
 *
 * If you bind an object of a sigc::trackable derived type to a functor
 * by reference, a slot assigned to the bind adaptor is cleared automatically
 * when the object goes out of scope.
 *
 * @par Example:
 * @code
 * struct bar : public sigc::trackable {} some_bar;
 * sigc::signal<void()> some_signal;
 * void foo(bar&);
 * some_signal.connect(sigc::bind(&foo, std::ref(some_bar)));
 *   // disconnected automatically if some_bar goes out of scope
 * @endcode
 *
 * @ingroup adaptors
 */

namespace internal
{

template <typename T_element>
struct TransformEachInvoker
{
  // We take T_element as non-const because invoke() is not const.
  // TODO: Take element as T_element&& ?
  constexpr static decltype(auto) transform(T_element& element) { return element.invoke(); }
};

} // namespace internal

/** Adaptor that binds arguments to the wrapped functor.
 * Use the convenience function sigc::bind() to create an instance of sigc::bind_functor.
 *
 * The following template arguments are used:
 * - @e I_location Zero-based position of the argument to fix (@p -1 for the last argument).
 * - @e T_bound Types of the bound argument.
 * - @e T_functor Type of the functor to wrap.
 *
 * @ingroup bind
 */
template <int I_location, typename T_functor, typename... T_bound>
struct bind_functor : public adapts<T_functor>
{
  /** Invokes the wrapped functor passing on the arguments.
   * bound_ is passed as the next argument.
   * @param arg Arguments to be passed on to the functor.
   * @return The return value of the functor invocation.
   */
  template <typename... T_arg>
  decltype(auto) operator()(T_arg&&... arg)
  {
    // For instance, if I_location is 1, and arg has 4 arguments,
    // we would want to call operator() with (arg0, bound, arg1, arg2).

    using tuple_type_args = std::tuple<type_trait_pass_t<T_arg>...>;
    const auto t_args = std::tuple<T_arg...>(std::forward<T_arg>(arg)...);
    constexpr auto t_args_size = std::tuple_size<tuple_type_args>::value;

    // Prevent calling tuple_start<> with values that will cause a compilation error.
    static_assert(I_location <= t_args_size,
      "I_location must be less than or equal to the number of arguments.");

    const auto t_start = internal::tuple_start<I_location>(t_args);
    const auto t_bound = internal::tuple_transform_each<internal::TransformEachInvoker>(bound_);
    const auto t_end = internal::tuple_end<t_args_size - I_location>(t_args);
    const auto t_with_bound = std::tuple_cat(t_start, t_bound, t_end);

    constexpr const auto seq =
      std::make_index_sequence<std::tuple_size<decltype(t_with_bound)>::value>();
    return call_functor_operator_parentheses(t_with_bound, seq);
  }

  /** Constructs a bind_functor object that binds an argument to the passed functor.
   * @param func Functor to invoke from operator()().
   * @param bound Argument to bind to the functor.
   */
  bind_functor(type_trait_take_t<T_functor> func, type_trait_take_t<T_bound>... bound)
  : adapts<T_functor>(func), bound_(bound...)
  {
  }

private:
  /// The arguments bound to the functor.
  std::tuple<bound_argument<T_bound>...> bound_;

  template <typename T, std::size_t... Is>
  decltype(auto) call_functor_operator_parentheses(T&& tuple, std::index_sequence<Is...>)
  {
    return (this->functor_)(std::get<Is>(std::forward<T>(tuple))...);
  }
};

/** Adaptor that binds argument(s) to the wrapped functor.
 * This template specialization fixes the last argument(s) of the wrapped functor.
 *
 * @ingroup bind
 */
template <typename T_functor, typename... T_type>
struct bind_functor<-1, T_functor, T_type...> : public adapts<T_functor>
{
public:
  /** Invokes the wrapped functor passing on the arguments.
   * bound_ is passed as the next argument.
   * @param arg Arguments to be passed on to the functor.
   * @return The return value of the functor invocation.
   */
  template <typename... T_arg>
  decltype(auto) operator()(T_arg&&... arg)
  {
    // For instance, if arg has 4 arguments,
    // we would want to call operator() with (arg0, arg1, arg2, bound).

    const auto t_args = std::tuple<T_arg...>(std::forward<T_arg>(arg)...);
    const auto t_bound = internal::tuple_transform_each<internal::TransformEachInvoker>(bound_);
    const auto t_with_bound = std::tuple_cat(t_args, t_bound);

    constexpr auto seq = std::make_index_sequence<std::tuple_size<decltype(t_with_bound)>::value>();
    return call_functor_operator_parentheses(t_with_bound, seq);
  }

  /** Constructs a bind_functor object that binds an argument to the passed functor.
   * @param func Functor to invoke from operator()().
   * @param bound Arguments to bind to the functor.
   */
  bind_functor(type_trait_take_t<T_functor> func, type_trait_take_t<T_type>... bound)
  : adapts<T_functor>(func), bound_(bound...)
  {
  }

  /// The argument bound to the functor.
  std::tuple<bound_argument<T_type>...> bound_;

private:
  template <typename T, std::size_t... Is>
  decltype(auto) call_functor_operator_parentheses(T&& tuple, std::index_sequence<Is...>)
  {
    return (this->functor_)(std::get<Is>(std::forward<T>(tuple))...);
  }
};

#ifndef DOXYGEN_SHOULD_SKIP_THIS
// template specialization of visitor<>::do_visit_each<>(action, functor):
/** Performs a functor on each of the targets of a functor.
 * The function overload for sigc::bind_functor performs a functor on the
 * functor and on the object instances stored in the sigc::bind_functor object.
 *
 * @ingroup bind
 */
template <int T_loc, typename T_functor, typename... T_bound>
struct visitor<bind_functor<T_loc, T_functor, T_bound...>>
{
  template <typename T_action>
  static void do_visit_each(
    const T_action& action, const bind_functor<T_loc, T_functor, T_bound...>& target)
  {
    sigc::visit_each(action, target.functor_);
    sigc::visit_each(action, std::get<0>(target.bound_));
  }
};

// template specialization of visitor<>::do_visit_each<>(action, functor):
/** Performs a functor on each of the targets of a functor.
 * The function overload for sigc::bind_functor performs a functor on the
 * functor and on the object instances stored in the sigc::bind_functor object.
 *
 * @ingroup bind
 */
template <typename T_functor, typename... T_type>
struct visitor<bind_functor<-1, T_functor, T_type...>>
{
  template <typename T_action>
  static void do_visit_each(
    const T_action& action, const bind_functor<-1, T_functor, T_type...>& target)
  {
    sigc::visit_each(action, target.functor_);

    sigc::internal::tuple_for_each<internal::TupleVisitorVisitEach>(target.bound_, action);
  }
};

#endif // DOXYGEN_SHOULD_SKIP_THIS

/** Creates an adaptor of type sigc::bind_functor which binds the passed argument to the passed
 * functor.
 * The optional template argument @e I_location specifies the zero-based
 * position of the argument to be fixed (@p -1 stands for the last argument).
 *
 * @param func Functor that should be wrapped.
 * @param b Arguments to bind to @e func.
 * @return Adaptor that executes @e func with the bound argument on invocation.
 *
 * @ingroup bind
 */
template <int I_location, typename T_functor, typename... T_bound>
inline decltype(auto)
bind(const T_functor& func, T_bound... b)
{
  return bind_functor<I_location, T_functor, T_bound...>(func, b...);
}

/** Creates an adaptor of type sigc::bind_functor which fixes the last arguments of the passed
 * functor.
 * This function overload fixes the last arguments of @e func.
 *
 * @param func Functor that should be wrapped.
 * @param b Arguments to bind to @e func.
 * @return Adaptor that executes func with the bound argument on invocation.
 *
 * @ingroup bind
 */
template <typename T_functor, typename... T_type>
inline decltype(auto)
bind(const T_functor& func, T_type... b)
{
  return bind_functor<-1, T_functor, T_type...>(func, b...);
}

} /* namespace sigc */

#endif /* SIGC_ADAPTORS_BIND_H */
