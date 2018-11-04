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

#ifndef SIGC_ADAPTORS_COMPOSE_H
#define SIGC_ADAPTORS_COMPOSE_H
#include <sigc++/adaptors/adapts.h>

namespace sigc
{

/** @defgroup compose compose()
 * sigc::compose() combines two or three arbitrary functors.
 * On invocation, parameters are passed on to one or two getter functor(s).
 * The return value(s) are then passed on to the setter function.
 *
 * @par Examples:
 * @code
 * float square_root(float a)  { return sqrtf(a); }
 * float sum(float a, float b) { return a+b; }
 * std::cout << sigc::compose(&square_root, &sum)(9, 16); // calls square_root(sum(3,6))
 * std::cout << sigc::compose(&sum, &square_root, &square_root)(9); // calls sum(square_root(9),
 * square_root(9))
 * @endcode
 *
 * The functor sigc::compose() returns can be passed directly into
 * sigc::signal::connect().
 *
 * @par Example:
 * @code
 * sigc::signal(float(float, float)> some_signal;
 * some_signal.connect(sigc::compose(&square_root, &sum));
 * @endcode
 *
 * @ingroup adaptors
 */

/** Adaptor that combines two functors.
 * Use the convenience function sigc::compose() to create an instance of sigc::compose1_functor.
 *
 * The following template arguments are used:
 * - @e T_setter Type of the setter functor to wrap.
 * - @e T_getter Type of the getter functor to wrap.
 *
 * @ingroup compose
 */
template <typename T_setter, typename T_getter>
struct compose1_functor : public adapts<T_setter>
{
  decltype(auto) operator()() { return this->functor_(get_()); }

  template <typename... T_arg>
  decltype(auto) operator()(T_arg&&... a)
  {
    return this->functor_(get_(std::forward<T_arg>(a)...));
  }

  /** Constructs a compose1_functor object that combines the passed functors.
   * @param setter Functor that receives the return values of the invocation of @e getter1 and
   * @e getter2.
   * @param getter Functor to invoke from operator()().
   */
  compose1_functor(const T_setter& setter, const T_getter& getter)
  : adapts<T_setter>(setter), get_(getter)
  {
  }

  T_getter get_; // public, so that visit_each() can access it
};

/** Adaptor that combines three functors.
 * Use the convenience function sigc::compose() to create an instance of sigc::compose2_functor.
 *
 * The following template arguments are used:
 * - @e T_setter Type of the setter functor to wrap.
 * - @e T_getter1 Type of the first getter functor to wrap.
 * - @e T_getter2 Type of the second getter functor to wrap.
 *
 * @ingroup compose
 */
template <typename T_setter, typename T_getter1, typename T_getter2>
struct compose2_functor : public adapts<T_setter>
{
  decltype(auto) operator()() { return this->functor_(get1_(), get2_()); }

  template <typename... T_arg>
  decltype(auto) operator()(T_arg... a)
  {
    return this->functor_(get1_(a...), get2_(a...));
  }

  /** Constructs a compose2_functor object that combines the passed functors.
   * @param setter Functor that receives the return values of the invocation of @e getter1 and
   * @e getter2.
   * @param getter1 Functor to invoke from operator()().
   * @param getter2 Functor to invoke from operator()().
   */
  compose2_functor(
    const T_setter& setter, const T_getter1& getter1, const T_getter2& getter2)
  : adapts<T_setter>(setter), get1_(getter1), get2_(getter2)
  {
  }

  T_getter1 get1_; // public, so that visit_each() can access it
  T_getter2 get2_; // public, so that visit_each() can access it
};

#ifndef DOXYGEN_SHOULD_SKIP_THIS
// template specialization of visitor<>::do_visit_each<>(action, functor):
/** Performs a functor on each of the targets of a functor.
 * The function overload for sigc::compose1_functor performs a functor on the
 * functors stored in the sigc::compose1_functor object.
 *
 * @ingroup compose
 */
template <typename T_setter, typename T_getter>
struct visitor<compose1_functor<T_setter, T_getter>>
{
  template <typename T_action>
  static void do_visit_each(
    const T_action& action, const compose1_functor<T_setter, T_getter>& target)
  {
    sigc::visit_each(action, target.functor_);
    sigc::visit_each(action, target.get_);
  }
};

// template specialization of visitor<>::do_visit_each<>(action, functor):
/** Performs a functor on each of the targets of a functor.
 * The function overload for sigc::compose2_functor performs a functor on the
 * functors stored in the sigc::compose2_functor object.
 *
 * @ingroup compose
 */
template <typename T_setter, typename T_getter1, typename T_getter2>
struct visitor<compose2_functor<T_setter, T_getter1, T_getter2>>
{
  template <typename T_action>
  static void do_visit_each(
    const T_action& action, const compose2_functor<T_setter, T_getter1, T_getter2>& target)
  {
    sigc::visit_each(action, target.functor_);
    sigc::visit_each(action, target.get1_);
    sigc::visit_each(action, target.get2_);
  }
};
#endif // DOXYGEN_SHOULD_SKIP_THIS

/** Creates an adaptor of type sigc::compose1_functor which combines two functors.
 *
 * @param setter Functor that receives the return value of the invocation of @e getter.
 * @param getter Functor to invoke from operator()().
 * @return Adaptor that executes @e setter with the value returned from invocation of @e
 * getter.
 *
 * @ingroup compose
 */
template <typename T_setter, typename T_getter>
inline compose1_functor<T_setter, T_getter>
compose(const T_setter& setter, const T_getter& getter)
{
  return compose1_functor<T_setter, T_getter>(setter, getter);
}

/** Creates an adaptor of type sigc::compose2_functor which combines three functors.
 *
 * @param setter Functor that receives the return values of the invocation of @e getter1 and
 * @e getter2.
 * @param getter1 Functor to invoke from operator()().
 * @param getter2 Functor to invoke from operator()().
 * @return Adaptor that executes @e setter with the values return from invocation of @e
 * getter1 and @e getter2.
 *
 * @ingroup compose
 */
template <typename T_setter, typename T_getter1, typename T_getter2>
inline compose2_functor<T_setter, T_getter1, T_getter2>
compose(const T_setter& setter, const T_getter1& getter1, const T_getter2& getter2)
{
  return compose2_functor<T_setter, T_getter1, T_getter2>(setter, getter1, getter2);
}

} /* namespace sigc */
#endif /* SIGC_ADAPTORS_COMPOSE_H */
