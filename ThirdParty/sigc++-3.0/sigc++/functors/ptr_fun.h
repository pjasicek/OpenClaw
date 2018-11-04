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

#ifndef SIGC_FUNCTORS_PTR_FUN_H
#define SIGC_FUNCTORS_PTR_FUN_H
#include <sigc++/type_traits.h>

namespace sigc
{

/** @defgroup ptr_fun ptr_fun()
 * ptr_fun() creates a functor from a pointer to a function.
 * If the function pointer is to an overloaded type, you must specify
 * the types using template arguments starting with the first argument.
 * It is not necessary to supply the return type.
 *
 * @par Example:
 * @code
 * void foo(int) {}
 * sigc::slot<void(int)> sl = sigc::ptr_fun(&foo);
 * @endcode
 *
 * @par Example:
 * @code
 * void foo(int) {}  // choose this one
 * void foo(float) {}
 * void foo(int, int) {}
 * sigc::slot<void(long)> sl = sigc::ptr_fun<void, int>(&foo);
 * @endcode
 *
 * ptr_fun() can also be used to convert a pointer to a static member
 * function to a functor, like so:
 *
 * @par Example:
 * @code
 * struct foo
 * {
 *   static void bar(int) {}
 * };
 * sigc::slot<void(int)> sl = sigc::ptr_fun(&foo::bar);
 * @endcode
 *
 * @ingroup sigcfunctors
 */

/** pointer_functor wraps existing non-member functions with, or without, arguments.
 * Use the convenience function ptr_fun() to create an instance of pointer_functor.
 *
 * The following template arguments are used:
 * - @e T_args... Argument types used in the definition of operator()().
 * - @e T_return The return type of operator()().
 *
 * @ingroup ptr_fun
 */
template <typename T_return, typename... T_args>
class pointer_functor;

template <typename T_return, typename... T_args>
class pointer_functor<T_return(T_args...)>
{
  using function_type = T_return (*)(T_args...);

protected:
  function_type func_ptr_;

public:

  /// Constructs an invalid functor.
  pointer_functor() = default;

  /** Constructs a pointer_functor2 object that wraps an existing function.
   * @param func Pointer to function that will be invoked from operator()().
   */
  explicit pointer_functor(function_type func) : func_ptr_(func) {}

  /** Execute the wrapped function.
   * @param a Arguments to be passed on to the function.
   * @return The return value of the function invocation.
   */
  T_return operator()(type_trait_take_t<T_args>... a) const { return func_ptr_(a...); }
};

/** Creates a functor of type sigc::pointer_functor which wraps an existing non-member function.
 * @param func Pointer to function that should be wrapped.
 * @return Functor that executes @e func on invocation.
 *
 * @ingroup ptr_fun
 */
template <typename T_return, typename... T_args>
inline decltype(auto) ptr_fun(T_return (*func)(T_args...))
{
  return pointer_functor<T_return(T_args...)>(func);
}

} /* namespace sigc */
#endif /* SIGC_FUNCTORS_PTR_FUN_H */
