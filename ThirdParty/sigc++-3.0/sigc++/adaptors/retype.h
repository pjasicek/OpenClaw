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

#ifndef SIGC_ADAPTORS_RETYPE_H
#define SIGC_ADAPTORS_RETYPE_H

#include <sigc++/adaptors/adapts.h>
#include <sigc++/functors/ptr_fun.h>
#include <sigc++/functors/mem_fun.h>
#include <sigc++/functors/slot.h>

namespace sigc
{

/** @defgroup retype retype(), retype_return()
 * sigc::retype() alters a sigc::pointer_functor, a sigc::mem_functor or a sigc::slot
 * in that it makes C-style casts to the functor's parameter types
 * of all parameters passed through operator()().
 *
 * Use this adaptor for inline conversion between numeric or other simple types.
 * @par Example:
 * @code
 * void foo(int);
 * sigc::retype(sigc::ptr_fun(&foo))(5.7F); // calls foo(5)
 * @endcode
 *
 * The functor that sigc::retype() returns can be passed directly into
 * sigc::signal::connect().
 *
 * @par Example:
 * @code
 * sigc::signal<void(float)> some_signal;
 * void foo(int);
 * some_signal.connect(sigc::retype(sigc::ptr_fun(&foo)));
 * @endcode
 *
 * This adaptor builds an exception in that it only works on sig::pointer_functor,
 * sigc::mem_functor and sigc::slot because it needs sophisticated information about
 * the parameter types that cannot be deduced from arbitrary functor types.
 *
 * sigc::retype_return() alters the return type of an arbitrary functor.
 * Like in sigc::retype() a C-style cast is performed. Usage sigc::retype_return() is
 * not restricted to libsigc++ functor types but you need to
 * specify the new return type as a template parameter.
 *
 * @par Example:
 * @code
 * float foo();
 * std::cout << sigc::retype_return<int>(&foo)(); // converts foo's return value to an integer
 * @endcode
 *
 * @ingroup adaptors
 */

/** Adaptor that performs C-style casts on the parameters passed on to the functor.
 * Use the convenience function sigc::retype() to create an instance of retype_functor.
 *
 * The following template arguments are used:
 * - @e T_functor Type of the functor to wrap.
 * - @e T_type Types of @e T_functor's arguments.
 *
 * @ingroup retype
 */
template <typename T_functor, typename... T_type>
struct retype_functor : public adapts<T_functor>
{
  template <typename... T_arg>
  decltype(auto) operator()(T_arg... a)
  {
    return this->functor_.template operator()<type_trait_take_t<T_type>...>(
      static_cast<T_type>(a)...);
  }

  /** Constructs a retype_functor object that performs C-style casts on the parameters passed on to
   * the functor.
   * @param functor Functor to invoke from operator()().
   */
  explicit retype_functor(type_trait_take_t<T_functor> functor) : adapts<T_functor>(functor)
  {
  }
};

#ifndef DOXYGEN_SHOULD_SKIP_THIS
// template specialization of visitor<>::do_visit_each<>(action, functor):
/** Performs a functor on each of the targets of a functor.
 * The function overload for sigc::retype_functor performs a functor on the
 * functor stored in the sigc::retype_functor object.
 *
 * @ingroup retype
 */
template <typename T_functor, typename... T_type>
struct visitor<retype_functor<T_functor, T_type...>>
{
  template <typename T_action>
  static void do_visit_each(
    const T_action& action, const retype_functor<T_functor, T_type...>& target)
  {
    sigc::visit_each(action, target.functor_);
  }
};
#endif // DOXYGEN_SHOULD_SKIP_THIS

// This one takes, for instance, a mem_functor or bound_mem_functor:
/** Creates an adaptor of type sigc::retype_functor which performs C-style casts on the parameters
 * passed on to the functor.
 *
 * @param functor Functor that should be wrapped.
 * @return Adaptor that executes @e functor performing C-style casts on the paramters passed on.
 *
 * @ingroup retype
 */
template <template <typename T_func, typename... T_arg> class T_functor, typename T_func, typename... T_arg>
inline decltype(auto)
retype(const T_functor<T_func, T_arg...>& functor)
{
  return retype_functor<T_functor<T_func, T_arg...>, T_arg...>(functor);
}

// This one takes, for instance, a pointer_functor or slot:
/** Creates an adaptor of type sigc::retype_functor which performs C-style casts on the parameters
 * passed on to the functor.
 *
 * @param functor Functor that should be wrapped.
 * @return Adaptor that executes @e functor performing C-style casts on the paramters passed on.
 *
 * @ingroup retype
 */
template <template <typename T_return, typename... T_arg> class T_functor, typename T_return, typename... T_arg>
inline decltype(auto)
retype(const T_functor<T_return(T_arg...)>& functor)
{
  return retype_functor<T_functor<T_return(T_arg...)>, T_arg...>(functor);
}

} /* namespace sigc */

#endif /* SIGC_ADAPTORS_RETYPE_H */
