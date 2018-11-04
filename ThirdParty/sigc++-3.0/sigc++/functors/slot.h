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

#ifndef SIGC_FUNCTORS_SLOT_H
#define SIGC_FUNCTORS_SLOT_H

#include <sigc++/trackable.h>
#include <sigc++/visit_each.h>
#include <sigc++/adaptors/adaptor_trait.h>
#include <sigc++/functors/slot_base.h>

#include <memory>

namespace sigc
{

namespace internal
{

/** A typed slot_rep.
 * A typed slot_rep holds a functor that can be invoked from
 * slot::operator()(). visit_each() is used to visit the functor's
 * targets that inherit trackable recursively and register the
 * notification callback. Consequently the slot_rep object will be
 * notified when some referred object is destroyed or overwritten.
 */
template <typename T_functor>
struct typed_slot_rep : public slot_rep
{
private:
  /* Use an adaptor type so that arguments can be passed as const references
   * through explicit template instantiation from slot_call#::call_it() */
  using adaptor_type = typename adaptor_trait<T_functor>::adaptor_type;
public:

  /** The functor contained by this slot_rep object. */
  std::unique_ptr<adaptor_type> functor_;

  /** Constructs an invalid typed slot_rep object.
   * The notification callback is registered using visit_each().
   * @param functor The functor contained by the new slot_rep object.
   */
  inline explicit typed_slot_rep(const T_functor& functor)
  : slot_rep(nullptr), functor_(std::make_unique<adaptor_type>(functor))
  {
    sigc::visit_each_trackable(slot_do_bind(this), *functor_);
  }

  inline typed_slot_rep(const typed_slot_rep& src)
  : slot_rep(src.call_), functor_(std::make_unique<adaptor_type>(*src.functor_))
  {
    sigc::visit_each_trackable(slot_do_bind(this), *functor_);
  }

  typed_slot_rep& operator=(const typed_slot_rep& src) = delete;

  typed_slot_rep(typed_slot_rep&& src) = delete;
  typed_slot_rep& operator=(typed_slot_rep&& src) = delete;

  ~typed_slot_rep() override
  {
    // Call destroy() non-virtually.
    // It's unwise to make virtual calls in a constructor or destructor.
    typed_slot_rep::destroy();
  }

private:
  /** Detaches the stored functor from the other referred trackables and destroys it.
   * This does not destroy the base slot_rep object.
   */
  void destroy() override
  {
    call_ = nullptr;
    if (functor_)
    {
      sigc::visit_each_trackable(slot_do_unbind(this), *functor_);
      functor_.reset(nullptr);
    }
    /* don't call disconnect() here: destroy() is either called
     * a) from the parent itself (in which case disconnect() leads to a segfault) or
     * b) from a parentless slot (in which case disconnect() does nothing)
     */
  }

  /** Makes a deep copy of the slot_rep object.
   * Deep copy means that the notification callback of the new
   * slot_rep object is registered in the referred trackables.
   * @return A deep copy of the slot_rep object.
   */
  slot_rep* clone() const override { return new typed_slot_rep(*this); }
};

/** Abstracts functor execution.
 * call_it() invokes a functor of type @e T_functor with a list of
 * parameters whose types are given by the template arguments.
 * address() forms a function pointer from call_it().
 *
 * The following template arguments are used:
 * - @e T_functor The functor type.
 * - @e T_return The return type of call_it().
 * - @e T_arg Argument types used in the definition of call_it().
 *
 */
template <typename T_functor, typename T_return, typename... T_arg>
struct slot_call
{
  /** Invokes a functor of type @p T_functor.
   * @param rep slot_rep object that holds a functor of type @p T_functor.
   * @param a Arguments to be passed on to the functor.
   * @return The return values of the functor invocation.
   */
  static T_return call_it(slot_rep* rep, type_trait_take_t<T_arg>... a_)
  {
    auto typed_rep = static_cast<typed_slot_rep<T_functor>*>(rep);
    return (*typed_rep->functor_).template operator()<type_trait_take_t<T_arg>...>(a_...);
  }

  /** Forms a function pointer from call_it().
   * @return A function pointer formed from call_it().
   */
  static hook address() { return reinterpret_cast<hook>(&call_it); }
};

} /* namespace internal */

// Because slot is opaque, visit_each() will not visit its internal members.
// Those members are not reachable by visit_each() after the slot has been
// constructed. But when a slot contains another slot, the outer slot will become
// the parent of the inner slot, with similar results. See the description of
// slot's specialization of the visitor struct.

/** Converts an arbitrary functor to a unified type which is opaque.
 * sigc::slot itself is a functor or, to be more precise, a closure. It contains
 * a single, arbitrary functor (or closure) that is executed in operator()().
 *
 * The template arguments determine the function signature of operator()():
 * - @e T_return The return type of operator()().
 * - @e T_arg Argument types used in the definition of operator()().
 *
 * For instance, to declare a slot that returns void and takes two parameters
 * of bool and int:
 * @code
 * sigc::slot<void(bool, int)> some_slot;
 * @endcode
 *
 * To use, simply assign the desired functor to the slot. If the functor
 * is not compatible with the parameter list defined with the template
 * arguments then compiler errors are triggered. When called, the slot
 * will invoke the functor with minimal copies.
 * block() and unblock() can be used to block the functor's invocation
 * from operator()() temporarily.
 *
 * @ingroup slot
 */
template <typename T_return, typename... T_arg>
class slot;

template <typename T_return, typename... T_arg>
class slot<T_return(T_arg...)> : public slot_base
{
public:
// TODO: using arg_type_ = type_trait_take_t<T_arg>;

#ifndef DOXYGEN_SHOULD_SKIP_THIS
private:
  using rep_type = internal::slot_rep;

public:
  using call_type = T_return (*)(rep_type*, type_trait_take_t<T_arg>...);
#endif

  /** Invoke the contained functor unless slot is in blocking state.
   * @param a Arguments to be passed on to the functor.
   * @return The return value of the functor invocation.
   */
  inline T_return operator()(type_trait_take_t<T_arg>... a) const
  {
    if (!empty() && !blocked())
      return (reinterpret_cast<call_type>(slot_base::rep_->call_))(slot_base::rep_, a...);
    return T_return();
  }

  inline slot() = default;

  /** Constructs a slot from an arbitrary functor.
   * @param func The desired functor the new slot should be assigned to.
   */
  template <typename T_functor>
  slot(const T_functor& func) : slot_base(new internal::typed_slot_rep<T_functor>(func))
  {
    // The slot_base:: is necessary to stop the HP-UX aCC compiler from being confused. murrayc.
    slot_base::rep_->call_ = internal::slot_call<T_functor, T_return, T_arg...>::address();
  }

  /** Constructs a slot, copying an existing one.
   * @param src The existing slot to copy.
   */
  slot(const slot& src) = default;

  /** Constructs a slot, moving an existing one.
   * If @p src is connected to a parent (e.g. a signal), it is copied, not moved.
   * @param src The existing slot to move or copy.
   */
  slot(slot&& src) : slot_base(std::move(src)) {}

  /** Overrides this slot, making a copy from another slot.
   * @param src The slot from which to make a copy.
   * @return @p this.
   */
  slot& operator=(const slot& src)
  = default;

  /** Overrides this slot, making a move from another slot.
   * If @p src is connected to a parent (e.g. a signal), it is copied, not moved.
   * @param src The slot from which to move or copy.
   * @return @p this.
   */
  slot& operator=(slot&& src)
  {
    slot_base::operator=(std::move(src));
    return *this;
  }
};

#ifndef DOXYGEN_SHOULD_SKIP_THIS
// template specialization of visitor<>::do_visit_each<>(action, functor):
/** Performs a functor on each of the targets of a functor.
 *
 * There are three function overloads for sigc::slot.
 *
 * The first two overloads are very specialized. They handle the (probably unusual)
 * case when the functor, stored in a slot, contains a slot. They are invoked from
 * the constructor, destructor or destroy() method of typed_slot_rep.
 * The first overload, called from the constructor of the outer slot, sets
 * the outer slot as the parent of the inner slot. The second overload, called from
 * the destructor or destroy() of the outer slot, unsets the parent of the inner slot.
 * When an object referenced from the inner slot is deleted, the inner slot calls
 * its slot_rep::disconnect(), which calls the outer slot's slot_rep::notify_slot_rep_invalidated().
 * The outer slot is informed just as if one of its directly referenced objects
 * had been deleted. Result: The outer slot is disconnected from its parent,
 * if any (for instance a sigc::signal).
 * See https://bugzilla.gnome.org/show_bug.cgi?id=755003
 *
 * The third overload is identical to do_visit_each() in visitor's primary template.
 *
 * @ingroup slot
 */
template <typename T_return, typename... T_arg>
struct visitor<slot<T_return, T_arg...>>
{
  static void do_visit_each(
    const internal::limit_trackable_target<internal::slot_do_bind>& action,
    const slot<T_return, T_arg...>& target)
  {
    if (target.rep_ && target.rep_->parent_ == nullptr)
      target.rep_->set_parent(action.action_.rep_, &internal::slot_rep::notify_slot_rep_invalidated);
  }

  static void do_visit_each(
    const internal::limit_trackable_target<internal::slot_do_unbind>& action,
    const slot<T_return, T_arg...>& target)
  {
    if (target.rep_ && target.rep_->parent_ == action.action_.rep_)
      target.rep_->unset_parent();
  }

  template <typename T_action>
  static void do_visit_each(const T_action& action, const slot<T_return, T_arg...>& target)
  {
    action(target);
  }
};
#endif // DOXYGEN_SHOULD_SKIP_THIS

} /* namespace sigc */

#endif /* SIGC_FUNCTORS_SLOT_H */
