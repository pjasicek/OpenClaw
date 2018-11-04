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

#ifndef SIGC_ADAPTORS_HIDE_H
#define SIGC_ADAPTORS_HIDE_H

#include <sigc++/adaptors/adapts.h>
#include <sigc++/tuple-utils/tuple_end.h>
#include <sigc++/tuple-utils/tuple_start.h>

namespace sigc
{

/** @defgroup hide hide(), hide_return()
 * sigc::hide() alters an arbitrary functor in that it adds a parameter
 * whose value is ignored on invocation of the returned functor.
 * Thus you can discard one argument of a signal.
 *
 * You may optionally specify the zero-based position of the parameter
 * to ignore as a template argument. The default is to ignore the last
 * parameter.
 * (A value of @p -1 adds a parameter at the end so sigc::hide<-1>() gives the same result as
 sigc::hide().)
 *
 * The type of the parameter can optionally be specified if not deduced.
 *
 * @par Examples:
 * @code
 * void foo(int, int);
 * // single argument hiding ...
 * sigc::hide(&foo)(1,2,3);     // adds a dummy parameter at the back and calls foo(1,2)
 * sigc::hide<-1>(&foo)(1,2,3); // same as sigc::hide(&foo)(1,2,3) (calls foo(1,2))
 * sigc::hide<0>(&foo)(1,2,3);  // adds a dummy parameter at the beginning and calls foo(2,3)
 * sigc::hide<1>(&foo)(1,2,3);  // adds a dummy parameter in the middle and calls foo(1,3)
 * sigc::hide<2>(&foo)(1,2,3);  // adds a dummy parameter at the back and calls foo(1,2)
 * @endcode
 *
 * The functor sigc::hide() returns can be directly passed into
 * sigc::signal::connect().
 *
 * @par Example:
 * @code
 * sigc::signal<void(int)> some_signal;
 * void foo();
 * some_signal.connect(sigc::hide(&foo));
 * @endcode
 *
 * sigc::hide() can be nested in order to discard multiple arguments.
 * @par Example:
 * @code
 * // multiple argument hiding ...
 * sigc::hide(sigc::hide(&foo))(1,2,3,4); // adds two dummy parameters at the back and calls
 foo(1,2)
 * @endcode

 * sigc::hide_return() alters an arbitrary functor by
 * dropping its return value, thus converting it to a void functor.
 *
 * @ingroup adaptors
 */

/** Adaptor that adds a dummy parameter to the wrapped functor.
 * Use the convenience function sigc::hide() to create an instance of sigc::hide_functor.
 *
 * The following template arguments are used:
 * - @e I_location Zero-based position of the dummy parameter (@p -1 for the last parameter).
 * - @e T_type Type of the dummy parameter.
 * - @e T_functor Type of the functor to wrap.
 *
 * @ingroup hide
 */
template <int I_location, typename T_functor>
struct hide_functor : public adapts<T_functor>
{
  /** Invokes the wrapped functor, ignoring the argument at index @e I_location (0-indexed).
   * @param a Arguments to be passed on to the functor, apart from the ignored argument.
   * @return The return value of the functor invocation.
   */
  template <typename... T_arg>
  decltype(auto) operator()(T_arg&&... a)
  {
    constexpr auto size = sizeof...(T_arg);
    constexpr auto index_ignore = (I_location == -1 ? size - 1 : I_location);
    const auto t = std::tuple<T_arg...>(std::forward<T_arg>(a)...);

    const auto t_start = internal::tuple_start<index_ignore>(t);
    const auto t_end = internal::tuple_end<size - index_ignore - 1>(t);
    const auto t_used = std::tuple_cat(t_start, t_end);

    constexpr auto size_used = size - 1;

    // TODO: Remove these? They are just here as a sanity check.
    static_assert(std::tuple_size<decltype(t_used)>::value == size_used, "Unexpected t_used size.");

    const auto seq = std::make_index_sequence<size_used>();
    return call_functor_operator_parentheses(t_used, seq);
  }

  /** Constructs a hide_functor object that adds a dummy parameter to the passed functor.
   * @param func Functor to invoke from operator()().
   */
  explicit hide_functor(const T_functor& func) : adapts<T_functor>(func) {}

private:
  // TODO_variadic: Replace this with std::experimental::apply() if that becomes standard
  // C++, or add our own implementation, to avoid code duplication.
  template <typename T_tuple, std::size_t... Is>
  decltype(auto) call_functor_operator_parentheses(T_tuple& tuple, std::index_sequence<Is...>)
  {
    return this->functor_.template operator()(std::get<Is>(tuple)...);
  }
};

#ifndef DOXYGEN_SHOULD_SKIP_THIS
// template specialization of visitor<>::do_visit_each<>(action, functor):
/** Performs a functor on each of the targets of a functor.
 * The function overload for sigc::hide_functor performs a functor on the
 * functor stored in the sigc::hide_functor object.
 *
 * @ingroup hide
 */
template <int I_location, typename T_functor>
struct visitor<hide_functor<I_location, T_functor>>
{
  template <typename T_action>
  static void do_visit_each(
    const T_action& action, const hide_functor<I_location, T_functor>& target)
  {
    sigc::visit_each(action, target.functor_);
  }
};
#endif // DOXYGEN_SHOULD_SKIP_THIS

/** Creates an adaptor of type sigc::hide_functor which adds a dummy parameter to the passed
 * functor.
 * The optional template argument @e I_location specifies the zero-based
 * position of the dummy parameter in the returned functor (@p -1 stands for the last parameter).
 *
 * @param func Functor that should be wrapped.
 * @return Adaptor that executes @e func, ignoring the value of the dummy parameter.
 *
 * @ingroup hide
 */
template <int I_location, typename T_functor>
inline decltype(auto)
hide(const T_functor& func)
{
  return hide_functor<I_location, T_functor>(func);
}

/** Creates an adaptor of type sigc::hide_functor which adds a dummy parameter to the passed
 * functor.
 * This overload adds a dummy parameter at the back of the functor's parameter list.
 *
 * @param func Functor that should be wrapped.
 * @return Adaptor that executes @e func, ignoring the value of the last parameter.
 *
 * @ingroup hide
 */
template <typename T_functor>
inline decltype(auto)
hide(const T_functor& func)
{
  return hide_functor<-1, T_functor>(func);
}

} /* namespace sigc */

#endif /* SIGC_ADAPTORS_HIDE_H */
