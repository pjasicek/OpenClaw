/*
 * Copyright 2005 - 2016, The libsigc++ Development Team
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifndef SIGC_BOUND_ARGUMENT_H
#define SIGC_BOUND_ARGUMENT_H

#include <sigc++/limit_reference.h>
#include <sigc++/reference_wrapper.h>

namespace sigc
{

/** A bound_argument<Foo> object stores a bound (for instance, with sigc::bind(), or
 * sigc::bind_return()) argument.
 *
 * If Foo is a wrapped reference to a class Bar (std::reference_wrapper<Bar>) then this
 * object is implemented on top of a limit_reference. When the slot is
 * invoked, the limit_reference::invoke() method provides the argument (a Bar&).
 * When the slot is visited (e.g. visit_each<>()), we simply visit the limit_reference,
 * which will visit the derived type, or a sigc::trackable base if necessary.
 *
 * Likewise, If Foo is a wrapped const reference to a class Bar (std::reference_wrapper<const Bar>)
 * then this object is implemented on top of a const_limit_reference.
 *
 * If Foo is something else (such as an argument that is bound by value) bound_argument just
 * stores a cop of that value, and both invoke() and visit() simply return it.
 *
 * This object is used by the bind_functor<> and bind_return_functor<> objects,
 * depending on whether the argument is bound as a parameter or as a return value.
 *
 * The general template implementation is used for parameters that are passed by value.
 * @e T_type The type of the bound argument.
 */
template <typename T_type>
class bound_argument
{
public:
  /** Constructor.
   * @param arg The argument to bind.
   */
  bound_argument(const T_type& arg) : visited_(arg) {}

  /** Retrieve the entity to visit in visit_each().
   * @return The bound argument.
   */
  inline const T_type& visit() const { return visited_; }

  /** Retrieve the entity to pass to the bound functor or return.
   * @return The bound argument.
   */
  inline T_type& invoke() { return visited_; }

private:
  /** The value of the argument.
   */
  T_type visited_;
};

// Template specialization:
/** bound_argument object for a bound argument that is passed by bind() or
 * returned by bind_return() by reference, specialized for std::reference_wrapper<> types.
 * @e T_wrapped The type of the bound argument.
 */
template <typename T_wrapped>
class bound_argument<std::reference_wrapper<T_wrapped>>
{
public:
  /** Constructor.
   * @param arg The argument to bind.
   */
  bound_argument(const std::reference_wrapper<T_wrapped>& arg)
  : visited_(unwrap(arg))
  {
  }

  /** Retrieve the entity to visit in visit_each().
   * @return The limited_reference to the bound argument.
   */
  inline const limit_reference<T_wrapped>& visit() const { return visited_; }

  /** Retrieve the entity to pass to the bound functor or return.
   * @return The bound argument.
   */
  inline T_wrapped& invoke() { return visited_.invoke(); }

private:
  /** The limited_reference to the bound argument.
   */
  limit_reference<T_wrapped> visited_;
};

/** bound_argument object for a bound argument that is passed by bind() or
 * returned by bind_return() by const reference, specialized for const reference_wrapper<> types.
 * - @e T_wrapped The type of the bound argument.
 */
template <typename T_wrapped>
class bound_argument<std::reference_wrapper<const T_wrapped>>
{
public:
  /** Constructor.
   * @param arg The argument to bind.
   */
  bound_argument(const std::reference_wrapper<const T_wrapped>& arg)
  : visited_(unwrap(arg))
  {
  }

  /** Retrieve the entity to visit in visit_each().
   * @return The const_limited_reference to the bound argument.
   */
  inline const limit_reference<const T_wrapped>& visit() const { return visited_; }

  /** Retrieve the entity to pass to the bound functor or return.
   * @return The bound argument.
   */
  inline const T_wrapped& invoke() { return visited_.invoke(); }

private:
  /** The const_limited_reference to the bound argument.
   */
  limit_reference<const T_wrapped> visited_;
};

#ifndef DOXYGEN_SHOULD_SKIP_THIS
/** Implementation of visitor<>::do_visit_each<>() specialized for the bound_argument class.
 * Call visit_each() on the entity returned by the bound_argument's visit()
 * method.
 * @tparam T_type The type of bound_argument.
 * @tparam T_action The type of functor to invoke.
 * @param action The functor to invoke.
 * @param arg The visited instance.
 */
template <typename T_type>
struct visitor<bound_argument<T_type>>
{
  template <typename T_action>
  static void do_visit_each(const T_action& action, const bound_argument<T_type>& arg)
  {
    sigc::visit_each(action, arg.visit());
  }
};
#endif // DOXYGEN_SHOULD_SKIP_THIS

} /* namespace sigc */

#endif /* SIGC_BOUND_ARGUMENT_H */
