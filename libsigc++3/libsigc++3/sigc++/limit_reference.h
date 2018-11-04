/*
 * Copyright 2005 - 2016, The libsigc++ Development Team
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

#ifndef SIGC_LIMIT_REFERENCE_H
#define SIGC_LIMIT_REFERENCE_H

#include <sigc++/visit_each.h>
#include <sigc++/type_traits.h>
#include <sigc++/trackable.h>

namespace sigc
{

/** A limit_reference<Foo> object stores a reference (Foo&), but makes sure that,
 * if Foo inherits from sigc::trackable, then visit_each<>() will "limit" itself to the
 * sigc::trackable reference instead of the derived reference. This avoids use of
 * a reference to the derived type when the derived destructor has run. That can be
 * a problem when using virtual inheritance.
 *
 * If Foo inherits from trackable then both the derived reference and the
 * sigc::trackable reference are stored, so we can later retrieve the sigc::trackable
 * reference without doing an implicit conversion. To retrieve the derived reference
 * (so that you invoke methods or members of it), use invoke(). To retrieve the trackable
 * reference (so that you can call visit_each() on it), you use visit().
 *
 * If Foo does not inherit from sigc::trackable then invoke() and visit() just return the
 * derived reference.
 *
 * This is used for bound (sigc::bind) slot parameters (via bound_argument), bound return values,
 * and, with mem_fun(), the reference to the handling object.
 *
 * - @e T_type The type of the reference.
 */
template <typename T_type,
  bool I_derives_trackable = std::is_base_of<trackable, std::decay_t<T_type>>::value>
class limit_reference
{
public:
  using reference_type = typename std::remove_volatile_t<T_type>;

  limit_reference() = delete;

  /** Constructor.
   * @param target The reference to limit.
   */
  explicit limit_reference(reference_type& target) : visited(target) {}

  limit_reference(const limit_reference& src) = default;
  limit_reference& operator=(const limit_reference& src) = default;

  limit_reference(limit_reference&& src) = default;
  limit_reference& operator=(limit_reference&& src) = default;

  /** Retrieve the entity to visit for visit_each().
   * Depending on the template specialization, this is either a derived reference, or
   * sigc::trackable& if T_type derives from sigc::trackable.
   * @return The reference.
   */
  inline const reference_type& visit() const { return visited; }

  /** Retrieve the reference.
   * This is always a reference to the derived instance.
   * @return The reference.
   */
  inline T_type& invoke() const { return visited; }

private:
  /** The reference.
   */
  reference_type& visited;
};

/** limit_reference object for a class that derives from trackable.
 * - @e T_type The type of the reference.
 */
template <typename T_type>
class limit_reference<T_type, true>
{
public:
  using reference_type = typename std::remove_volatile_t<T_type>;

  /** Constructor.
   * @param target The reference to limit.
   */
  limit_reference(reference_type& target) : visited(target), invoked(target) {}

  /** Retrieve the entity to visit for visit_each().
   * Depending on the template specialization, this is either a derived reference, or
   * sigc::trackable& if T_type derives from sigc::trackable.
   * @return The reference.
   */
  inline const trackable& visit() const { return visited; }

  /** Retrieve the reference.
   * This is always a reference to the derived instance.
   * @return The reference.
   */
  inline T_type& invoke() const { return invoked; }

private:
  using trackable_type =
    typename std::conditional_t<std::is_const<reference_type>::value, const trackable, trackable>;

  /** The trackable reference.
   */
  trackable_type& visited;

  /** The reference.
   */
  reference_type& invoked;
};

#ifndef DOXYGEN_SHOULD_SKIP_THIS
/** Implementation of visitor specialized for the $1limit_reference
 * class, to call visit_each() on the entity returned by the $1limit_reference's
 * visit() method.
 * @tparam T_type The type of the reference
 * @tparam T_action The type of functor to invoke.
 * @param action The functor to invoke.
 * @param target The visited instance.
 */
template <typename T_type>
struct visitor<limit_reference<T_type>>
{
  template <typename T_action>
  static void do_visit_each(const T_action& action, const limit_reference<T_type>& target)
  {
    sigc::visit_each(action, target.visit());
  }
};
#endif // DOXYGEN_SHOULD_SKIP_THIS

} /* namespace sigc */

#endif /* SIGC_LIMIT_REFERENCE_H */
