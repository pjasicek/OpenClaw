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

#ifndef SIGC_ADAPTORS_BIND_RETURN_H
#define SIGC_ADAPTORS_BIND_RETURN_H
#include <sigc++/adaptors/adapts.h>
#include <sigc++/adaptors/bound_argument.h>

namespace sigc
{

/** Adaptor that fixes the return value of the wrapped functor.
 * Use the convenience function sigc::bind_return() to create an instance of
 * sigc::bind_return_functor.
 *
 * The following template arguments are used:
 * - @e T_return Type of the fixed return value.
 * - @e T_functor Type of the functor to wrap.
 *
 * @ingroup bind
 */
template <typename T_return, typename T_functor>
struct bind_return_functor : public adapts<T_functor>
{
  /** Invokes the wrapped functor dropping its return value.
   * @return The fixed return value.
   */
  typename unwrap_reference<T_return>::type operator()();

  /** Invokes the wrapped functor passing on the arguments.
   * @param a Arguments to be passed on to the functor.
   * @return The fixed return value.
   */
  template <typename... T_arg>
  inline typename unwrap_reference<T_return>::type operator()(T_arg... a)
  {
    this->functor_.template operator()<type_trait_pass_t<T_arg>...>(a...);
    return ret_value_.invoke();
  }

  /** Constructs a bind_return_functor object that fixes the return value to @p ret_value.
   * @param functor Functor to invoke from operator()().
   * @param ret_value Value to return from operator()().
   */
  bind_return_functor(
    type_trait_take_t<T_functor> functor, type_trait_take_t<T_return> ret_value)
  : adapts<T_functor>(functor), ret_value_(ret_value)
  {
  }

  /// The fixed return value.
  bound_argument<T_return> ret_value_; // public, so that visit_each() can access it
};

template <typename T_return, typename T_functor>
typename unwrap_reference<T_return>::type
bind_return_functor<T_return, T_functor>::operator()()
{
  this->functor_();
  return ret_value_.invoke();
}

#ifndef DOXYGEN_SHOULD_SKIP_THIS
// template specialization of visitor<>::do_visit_each<>(action, functor):
/** Performs a functor on each of the targets of a functor.
 * The function overload for sigc::bind_return_functor performs a functor on the
 * functor and on the object instance stored in the sigc::bind_return_functor object.
 *
 * @ingroup bind
 */
template <typename T_return, typename T_functor>
struct visitor<bind_return_functor<T_return, T_functor>>
{
  template <typename T_action>
  static void do_visit_each(
    const T_action& action, const bind_return_functor<T_return, T_functor>& target)
  {
    sigc::visit_each(action, target.ret_value_);
    sigc::visit_each(action, target.functor_);
  }
};
#endif // DOXYGEN_SHOULD_SKIP_THIS

/** Creates an adaptor of type sigc::bind_return_functor which fixes the return value of the passed
 * functor to the passed argument.
 *
 * @param functor Functor that should be wrapped.
 * @param ret_value Argument to fix the return value of @e functor to.
 * @return Adaptor that executes @e functor on invocation and returns @e ret_value.
 *
 * @ingroup bind
 */
template <typename T_return, typename T_functor>
inline bind_return_functor<T_return, T_functor>
bind_return(const T_functor& functor, T_return ret_value)
{
  return bind_return_functor<T_return, T_functor>(functor, ret_value);
}

} /* namespace sigc */
#endif /* SIGC_ADAPTORS_BIND_RETURN_H */
