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

#ifndef SIGC_ADAPTORS_RETYPE_RETURN_H
#define SIGC_ADAPTORS_RETYPE_RETURN_H
#include <sigc++/adaptors/adapts.h>

namespace sigc
{

/** Adaptor that performs a C-style cast on the return value of a functor.
 * Use the convenience function sigc::retype_return() to create an instance of
 * retype_return_functor.
 *
 * The following template arguments are used:
 * - @e T_return Target type of the C-style cast.
 * - @e T_functor Type of the functor to wrap.
 *
 * @ingroup retype
 */
template <typename T_return, typename T_functor>
struct retype_return_functor : public adapts<T_functor>
{
  T_return operator()();

  template <typename... T_arg>
  inline T_return operator()(T_arg&&... a)
  {
    return T_return(this->functor_.template operator() < T_arg... > (std::forward<T_arg>(a)...));
  }

  retype_return_functor() = default;

  /** Constructs a retype_return_functor object that perform a C-style cast on the return value of
   * the passed functor.
   * @param functor Functor to invoke from operator()().
   */
  explicit retype_return_functor(type_trait_take_t<T_functor> functor)
  : adapts<T_functor>(functor)
  {
  }
};

template <typename T_return, typename T_functor>
T_return
retype_return_functor<T_return, T_functor>::operator()()
{
  return T_return(this->functor_());
}

/** Adaptor that performs a C-style cast on the return value of a functor.
 * This template specialization is for a void return. It drops the return value of the functor it
 * invokes.
 * Use the convenience function sigc::hide_return() to create an instance of
 * sigc::retype_return_functor<void>.
 *
 * @ingroup retype
 */
/* The void specialization is needed because of explicit cast to T_return.
 */
template <typename T_functor>
struct retype_return_functor<void, T_functor> : public adapts<T_functor>
{
  void operator()();

  template <typename... T_arg>
  inline void operator()(T_arg... a)
  {
    this->functor_.template operator()<T_arg...>(a...);
  }

  retype_return_functor() = default;
  retype_return_functor(type_trait_take_t<T_functor> functor) : adapts<T_functor>(functor) {}
};

template <typename T_functor>
void
retype_return_functor<void, T_functor>::operator()()
{
  this->functor_();
}

#ifndef DOXYGEN_SHOULD_SKIP_THIS
// template specialization of visitor<>::do_visit_each<>(action, functor):
/** Performs a functor on each of the targets of a functor.
 * The function overload for sigc::retype_return_functor performs a functor on the
 * functor stored in the sigc::retype_return_functor object.
 *
 * @ingroup retype
 */
template <typename T_return, typename T_functor>
struct visitor<retype_return_functor<T_return, T_functor>>
{
  template <typename T_action>
  static void do_visit_each(
    const T_action& action, const retype_return_functor<T_return, T_functor>& target)
  {
    sigc::visit_each(action, target.functor_);
  }
};
#endif // DOXYGEN_SHOULD_SKIP_THIS

/** Creates an adaptor of type sigc::retype_return_functor which performs a C-style cast on the
 * return value of the passed functor.
 * The template argument @e T_return specifies the target type of the cast.
 *
 * @param functor Functor that should be wrapped.
 * @return Adaptor that executes @e functor performing a C-style cast on the return value.
 *
 * @ingroup retype
 */
template <typename T_return, typename T_functor>
inline retype_return_functor<T_return, T_functor>
retype_return(const T_functor& functor)
{
  return retype_return_functor<T_return, T_functor>(functor);
}

/** Creates an adaptor of type sigc::retype_return_functor which drops the return value of the
 * passed functor.
 *
 * @param functor Functor that should be wrapped.
 * @return Adaptor that executes @e functor dropping its return value.
 *
 * @ingroup hide
 */
template <typename T_functor>
inline retype_return_functor<void, T_functor>
hide_return(const T_functor& functor)
{
  return retype_return_functor<void, T_functor>(functor);
}

} /* namespace sigc */
#endif /* SIGC_ADAPTORS_RETYPE_RETURN_H */
