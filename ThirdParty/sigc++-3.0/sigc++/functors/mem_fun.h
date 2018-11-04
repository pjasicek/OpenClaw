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

#ifndef SIGC_FUNCTORS_MEM_FUN_H
#define SIGC_FUNCTORS_MEM_FUN_H
#include <sigc++/type_traits.h>
#include <sigc++/limit_reference.h>
#include <sigc++/member_method_trait.h>

// implementation notes:
//  - we do not use bind here, because it would introduce
//    an extra copy and complicate the header include order if bind is
//    to have automatic conversion for member pointers.

namespace sigc
{

/** @defgroup mem_fun mem_fun()
 * mem_fun() Creates a functor from a pointer to a method.
 *
 * Optionally, a reference or pointer to an object can be bound to the functor.
 *
 * @note If the object type inherits from sigc::trackable, and the
 * functor returned from mem_fun() is assigned to a sigc::slot, the functor
 * will be automatically cleared when the object goes out of scope. Invoking
 * that slot will then have no effect and will not try to use the destroyed
 * instance.
 *
 * If the member function pointer is to an overloaded type, you must specify
 * the types using template arguments starting with the first argument.
 * It is not necessary to supply the return type.
 *
 * @par Example:
 * @code
 * struct foo : public sigc::trackable
 * {
 *   void bar(int) {}
 * };
 * foo my_foo;
 * sigc::slot<void(int)> sl = sigc::mem_fun(my_foo, &foo::bar);
 * // Note: f is not a slot. It will not be invalidated when my_foo is deleted.
 * auto f = sigc::mem_fun(my_foo, &foo::bar); // Usually not what you want.
 * @endcode
 *
 * For const methods mem_fun() takes a const reference or pointer to an object.
 *
 * @par Example:
 * @code
 * struct foo : public sigc::trackable
 * {
 *   void bar(int) const {}
 * };
 * const foo my_foo;
 * sigc::slot<void(int)> sl = sigc::mem_fun(my_foo, &foo::bar);
 * @endcode
 *
 * Use mem_fun#() if there is an ambiguity as to the number of arguments.
 *
 * @par Example:
 * @code
 * struct foo : public sigc::trackable
 * {
 *   void bar(int) {}
 *   void bar(float) {}
 *   void bar(int, int) {}
 * };
 * foo my_foo;
 * sigc::slot<void(int)> sl = sigc::mem_fun1<int>(my_foo, &foo::bar);
 * @endcode
 *
 * @ingroup sigcfunctors
 */

template <typename T_func, typename... T_arg>
class mem_functor
{
public:
  using object_type = typename internal::member_method_class<T_func>::type;

  using function_type = T_func;

  using obj_type_with_modifier =
    typename std::conditional_t<internal::member_method_is_const<T_func>::value, const object_type,
      object_type>;

  /// Constructs an invalid functor.
  mem_functor() : func_ptr_(nullptr) {}

  /** Constructs a mem_functor object that wraps the passed method.
   * @param func Pointer to method will be invoked from operator()().
   */
  explicit mem_functor(function_type func) : func_ptr_(func) {}

  /** Execute the wrapped method operating on the passed instance.
   * @param obj Reference to instance the method should operate on.
   * @param a Arguments to be passed on to the method.
   * @return The return value of the method invocation.
   */
  decltype(auto) operator()(obj_type_with_modifier& obj, type_trait_take_t<T_arg>... a) const
  {
    return (obj.*func_ptr_)(a...);
  }

protected:
  function_type func_ptr_;
};

template <typename T_func, typename... T_arg>
class bound_mem_functor : mem_functor<T_func, T_arg...>
{
  using base_type = mem_functor<T_func, T_arg...>;

public:
  using function_type = typename base_type::function_type;

  using object_type = typename base_type::object_type;

  using obj_type_with_modifier =
    typename std::conditional_t<internal::member_method_is_const<T_func>::value, const object_type,
      object_type>;
  using T_limit_reference =
    typename std::conditional_t<internal::member_method_is_const<T_func>::value,
      limit_reference<const object_type>, limit_reference<object_type>>;

  /** Constructs a bound_mem_functor object that wraps the passed method.
   * @param obj Reference to instance the method will operate on.
   * @param func Pointer to method will be invoked from operator()().
   */
  bound_mem_functor(obj_type_with_modifier& obj, function_type func)
  : base_type(func), obj_(obj)
  {
  }

  /** Execute the wrapped method operating on the stored instance.
   * @param a Arguments to be passed on to the method.
   * @return The return value of the method invocation.
   */
  decltype(auto) operator()(type_trait_take_t<T_arg>... a) const
  {
    return (obj_.invoke().*(this->func_ptr_))(a...);
  }

  // protected:
  // Reference to stored object instance.
  // This is the handler object, such as TheObject in void TheObject::signal_handler().
  T_limit_reference obj_;
};

#ifndef DOXYGEN_SHOULD_SKIP_THIS
// template specialization of visitor<>::do_visit_each<>(action, functor):
/** Performs a functor on each of the targets of a functor.
 * The function overload for sigc::bound_$1mem_functor performs a functor
 * on the object instance stored in the sigc::bound_$1mem_functor object.
 *
 * @ingroup mem_fun
 */
template <typename T_func, typename... T_arg>
struct visitor<bound_mem_functor<T_func, T_arg...>>
{
  template <typename T_action>
  static void do_visit_each(
    const T_action& action, const bound_mem_functor<T_func, T_arg...>& target)
  {
    sigc::visit_each(action, target.obj_);
  }
};
#endif // DOXYGEN_SHOULD_SKIP_THIS

/** Creates a functor of type sigc::mem_functor which wraps a  method.
 * @param func Pointer to method that should be wrapped.
 * @return Functor that executes func on invocation.
 *
 * @ingroup mem_fun
 */
template <typename T_return, typename T_obj, typename... T_arg>
inline decltype(auto)
mem_fun(T_return (T_obj::*func)(T_arg...))
{
  return mem_functor<T_return (T_obj::*)(T_arg...), T_arg...>(func);
}

/** Creates a functor of type sigc::const_mem_functor which wraps a const method.
 * @param func Pointer to method that should be wrapped.
 * @return Functor that executes func on invocation.
 *
 * @ingroup mem_fun
 */
template <typename T_return, typename T_obj, typename... T_arg>
inline decltype(auto)
mem_fun(T_return (T_obj::*func)(T_arg...) const)
{
  return mem_functor<T_return (T_obj::*)(T_arg...) const, T_arg...>(func);
}

/** Creates a functor of type sigc::volatile_mem_functor which wraps a volatile method.
 * @param func Pointer to method that should be wrapped.
 * @return Functor that executes func on invocation.
 *
 * @ingroup mem_fun
 */
template <typename T_return, typename T_obj, typename... T_arg>
inline decltype(auto)
mem_fun(T_return (T_obj::*func)(T_arg...) volatile)
{
  return mem_functor<T_return (T_obj::*)(T_arg...) volatile, T_arg...>(func);
}

/** Creates a functor of type sigc::const_volatile_mem_functor which wraps a const volatile method.
 * @param func Pointer to method that should be wrapped.
 * @return Functor that executes func on invocation.
 *
 * @ingroup mem_fun
 */
template <typename T_return, typename T_obj, typename... T_arg>
inline decltype(auto)
mem_fun(T_return (T_obj::*func)(T_arg...) const volatile)
{
  return mem_functor<T_return (T_obj::*)(T_arg...) const volatile, T_arg...>(func);
}

/** Creates a functor of type sigc::bound_mem_functor which encapsulates a method and an object
 * instance.
 * @param obj Reference to object instance the functor should operate on.
 * @param func Pointer to method that should be wrapped.
 * @return Functor that executes @e func on invocation.
 *
 * @ingroup mem_fun
 */
template <typename T_return, typename T_obj, typename T_obj2, typename... T_arg>
inline decltype(auto)
mem_fun(/**/ T_obj& obj, T_return (T_obj2::*func)(T_arg...))
{
  return bound_mem_functor<T_return (T_obj::*)(T_arg...), T_arg...>(obj, func);
}

/** Creates a functor of type sigc::bound_const_mem_functor which encapsulates a method and an
 * object instance.
 * @param obj Reference to object instance the functor should operate on.
 * @param func Pointer to method that should be wrapped.
 * @return Functor that executes @e func on invocation.
 *
 * @ingroup mem_fun
 */
template <typename T_return, typename T_obj, typename T_obj2, typename... T_arg>
inline decltype(auto)
mem_fun(/*const*/ T_obj& obj, T_return (T_obj2::*func)(T_arg...) const)
{
  return bound_mem_functor<T_return (T_obj::*)(T_arg...) const, T_arg...>(obj, func);
}

/** Creates a functor of type sigc::bound_volatile_mem_functor which encapsulates a method and an
 * object instance.
 * @param obj Reference to object instance the functor should operate on.
 * @param func Pointer to method that should be wrapped.
 * @return Functor that executes @e func on invocation.
 *
 * @ingroup mem_fun
 */
template <typename T_return, typename T_obj, typename T_obj2, typename... T_arg>
inline decltype(auto)
mem_fun(/**/ T_obj& obj, T_return (T_obj2::*func)(T_arg...) volatile)
{
  return bound_mem_functor<T_return (T_obj::*)(T_arg...) volatile, T_arg...>(obj, func);
}

/** Creates a functor of type sigc::bound_const_volatile_mem_functor which encapsulates a method and
 * an object instance.
 * @param obj Reference to object instance the functor should operate on.
 * @param func Pointer to method that should be wrapped.
 * @return Functor that executes @e func on invocation.
 *
 * @ingroup mem_fun
 */
template <typename T_return, typename T_obj, typename T_obj2, typename... T_arg>
inline decltype(auto)
mem_fun(/*const*/ T_obj& obj, T_return (T_obj2::*func)(T_arg...) const volatile)
{
  return bound_mem_functor<T_return (T_obj::*)(T_arg...) const volatile, T_arg...>(obj, func);
}

} /* namespace sigc */
#endif /* SIGC_FUNCTORS_MEM_FUN_H */
