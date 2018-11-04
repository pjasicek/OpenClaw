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
 *
 */
#ifndef SIGC_SLOT_BASE_HPP
#define SIGC_SLOT_BASE_HPP

#include <sigc++config.h>
#include <sigc++/trackable.h>

namespace sigc
{

namespace internal
{

using hook = void* (*)(void*);

/** Internal representation of a slot.
 * Derivations of this class can be considered as a link
 * between a slot and the functor that the slot should
 * execute in operator(). This link is needed because in
 * libsigc++ the slot doesn't necessarily have exactly the
 * same function signature as the functor, thus allowing for
 * implicit conversions.
 *
 * The base class slot_rep serves the purpose to
 * - form a common pointer type (slot_rep*),
 * - offer the possibility to create duplicates (clone()),
 * - offer a notification callback (notify_slot_rep_invalidated()),
 * - implement some of slot_base's interface that depends
 *   on the notification callback, i.e.
 *   -# the possibility to set a single parent with a callback
 *      (set_parent()) that is executed from notify_slot_rep_invalidated(),
 *   -# a generic function pointer, call_, that is simply
 *      set to zero in notify_slot_rep_invalidated() to invalidate the slot.
 *
 * slot_rep inherits trackable so that connection objects can
 * refer to the slot and are notified when the slot is destroyed.
 */
struct SIGC_API slot_rep : public trackable
{
public:
  slot_rep(const slot_rep& src) = delete;
  slot_rep& operator=(const slot_rep& src) = delete;

  slot_rep(slot_rep&& src) = delete;
  slot_rep& operator=(slot_rep&& src) = delete;

  /* NB: Instead of slot_rep we could inherit slot_base from trackable.
   * However, a simple benchmark seems to indicate that this slows
   * down dereferencing of slot list iterators. Martin. */
  // TODO: Try this now? murrayc.

  inline slot_rep(hook call__) noexcept
    : call_(call__),
      cleanup_(nullptr),
      parent_(nullptr)
  {
  }

  virtual ~slot_rep() {}

// only MSVC needs this to guarantee that all new/delete are executed from the DLL module
#ifdef SIGC_NEW_DELETE_IN_LIBRARY_ONLY
  void* operator new(size_t size_);
  void operator delete(void* p);
#endif

  /** Destroys the slot_rep object (but doesn't delete it).
   */
  virtual void destroy() = 0;

  /** Makes a deep copy of the slot_rep object.
   * @return A deep copy of the slot_rep object.
   */
  virtual slot_rep* clone() const = 0;

  /** Set the parent with a callback.
   * slots have one parent exclusively.
   * @param parent The new parent.
   * @param cleanup The callback to execute from notify_slot_rep_invalidated().
   */
  inline void set_parent(notifiable* parent, notifiable::func_destroy_notify cleanup) noexcept
  {
    parent_ = parent;
    cleanup_ = cleanup;
  }

  /** See set_parent().
   *
   */
  inline void unset_parent() noexcept
  {
    parent_ = nullptr;
    cleanup_ = nullptr;
  }

  /// Invalidates the slot and executes the parent's cleanup callback.
  void disconnect();

  /** Callback that invalidates the slot.
   * This callback is registered in every object of a trackable
   * inherited type that is referred by this slot_rep object.
   * It is executed when the slot becomes invalid because of some
   * referred object dying.
   * @param data The slot_rep object that is becoming invalid (@p this).
   */
  static void notify_slot_rep_invalidated(notifiable* data);

public:
  /// Callback that invokes the contained functor.
  /* This can't be a virtual function since number of arguments
   * must be flexible. We use function pointers to slot_call::call_it()
   * instead. call_ is set to zero to indicate that the slot is invalid.
   */
  hook call_;

  /** Callback of parent_. */
  notifiable::func_destroy_notify cleanup_;

  /** Parent object whose callback cleanup_ is executed on notification. */
  notifiable* parent_;
};

/** Functor used to add a dependency to a trackable.
 * Consequently slot_rep::notify_slot_rep_invalidated() gets executed when the
 * trackable is destroyed or overwritten.
 */
struct SIGC_API slot_do_bind
{
  /** The slot_rep object trackables should notify on destruction. */
  slot_rep* rep_;

  /** Construct a slot_do_bind functor.
   * @param rep The slot_rep object trackables should notify on destruction.
   */
  inline explicit slot_do_bind(slot_rep* rep) noexcept : rep_(rep) {}

  /** Adds a dependency to @p t.
   * @param t The trackable object to add a callback to.
   */
  inline void operator()(const trackable& t) const
  {
    t.add_destroy_notify_callback(rep_, &slot_rep::notify_slot_rep_invalidated);
  }
};

/// Functor used to remove a dependency from a trackable.
struct SIGC_API slot_do_unbind
{
  /** The slot_rep object trackables don't need to notify on destruction any more. */
  slot_rep* rep_;

  /** Construct a slot_do_unbind functor.
   * @param rep The slot_rep object trackables don't need to notify on destruction any more.
   */
  inline explicit slot_do_unbind(slot_rep* rep) noexcept : rep_(rep) {}

  /** Removes a dependency from @p t.
   * @param t The trackable object to remove the callback from.
   */
  inline void operator()(const trackable& t) const { t.remove_destroy_notify_callback(rep_); }
};

} // namespace internal

/** @defgroup slot Slots
 * Slots are type-safe representations of callback methods and functions.
 * A slot can be constructed from any function object or function, regardless of
 * whether it is a global function, a member method, static, or virtual.
 *
 * @section slots-creating Creating Slots
 *
 * Use the sigc::mem_fun() or sigc::ptr_fun() template functions to get a sigc::slot, like so:
 * @code
 * sigc::slot<void(int)> sl = sigc::mem_fun(someobj, &SomeClass::somemethod);
 * @endcode
 * or
 * @code
 * sigc::slot<void(int)> sl = sigc::ptr_fun(&somefunction);
 * @endcode
 * or, in gtkmm,
 * @code
 * m_Button.signal_clicked().connect( sigc::mem_fun(*this, &MyWindow::on_button_clicked) );
 * @endcode
 *
 * The compiler will complain if SomeClass::somemethod, etc. have the wrong signature.
 *
 * You can also pass slots as method parameters where you might normally pass a function pointer.
 *
 * @section slots-auto-disconnect Member Methods and Automatic Disconnection
 *
 * See @ref mem_fun "sigc::mem_fun()" about deriving from sigc::trackable to prevent member
 * methods from being called after the instance has been destroyed.
 *
 * @section slots-auto auto
 *
 * sigc::mem_fun() and sigc::ptr_fun() return functors, but those functors are
 * not slots.
 * @code
 * sigc::slot<void(int)> sl = sigc::mem_fun(someobj, &SomeClass::somemethod);
 * @endcode
 * is not equivalent to
 * @code
 * auto sl = sigc::mem_fun(someobj, &SomeClass::somemethod); // Not a slot!
 * @endcode
 *
 * If you don't explicitly use a sigc::slot then the slot could call a method
 * on an instance after it has been destroyed even if the method is in a class
 * that derives from sigc::trackable.
 *
 * @section slots-with-lambdas C++ Lambdas
 *
 * A C++11 lambda expression is a functor (function object). It is automatically
 * wrapped in a slot, if it is connected to a signal.
 * @code
 * auto on_response = [&someobj] (int response_id)
 *   {
 *     someobj.somemethod(response_id);
 *     somefunction(response_id);
 *   };
 * m_Dialog.signal_response().connect(on_response);
 * @endcode
 *
 * If you connect a C++11 lambda expression or a std::function<> instance to
 * a signal or assign it to a slot, if your functor contains references to
 * sigc::trackable derived objects, those objects will not be tracked,
 * unless you also use sigc::track_obj().
 *
 * @ingroup sigcfunctors
 */

/** Base type for slots.
 * slot_base integrates most of the interface of the derived
 * sigc::slot templates. slots
 * can be connected to signals, be disconnected at some later point
 * (disconnect()) and temporarily be blocked (block(), unblock()).
 * The validity of a slot can be tested with empty().
 *
 * The internal representation of a sigc::internal::slot_rep derived
 * type is built from slot_base's derivations. set_parent() is used to
 * register a notification callback that is executed when the slot gets
 * invalid. add_destroy_notify_callback() is used by connection objects
 * to add a notification callback that is executed on destruction.
 *
 * @ingroup slot
 */
class SIGC_API slot_base
{
  using rep_type = internal::slot_rep;

  // Move operations are not declared noexcept because
  // 1. they may copy instead of move
  // 2. when they don't copy, they call src.rep_->notify_callbacks(), which
  //    may throw an exception.
public:
  /// Constructs an empty slot.
  slot_base() noexcept;

  /** Constructs a slot from an existing slot_rep object.
   * @param rep The slot_rep object this slot should contain.
   */
  explicit slot_base(rep_type* rep) noexcept;

  /** Constructs a slot, copying an existing one.
   * @param src The existing slot to copy.
   */
  slot_base(const slot_base& src);

  /** Constructs a slot, moving an existing one.
   * If @p src is connected to a parent (e.g. a signal), it is copied, not moved.
   * @param src The existing slot to move or copy.
   */
  slot_base(slot_base&& src);

  ~slot_base();

  /** Tests whether a slot is null, because the default constructor was used.
   * Test a slot for null like so:
   * @code
   * if(slot)
   *  do_something()
   * @endcode
   */
  explicit operator bool() const noexcept;

  using func_destroy_notify = notifiable::func_destroy_notify;

  /** Sets the parent of this slot.
   * This function is used by signals to register a notification callback.
   * This notification callback is executed when the slot becomes invalid
   * because of some referred object dying.
   * @param parent The new parent.
   * @param cleanup The notification callback.
   */
  void set_parent(notifiable* parent, notifiable::func_destroy_notify cleanup) const noexcept;

  /** Add a callback that is executed (notified) when the slot is destroyed.
   * This function is used internally by connection objects.
   * @param data Passed into func upon notification.
   * @param func Callback executed upon destruction of the object.
   */
  void add_destroy_notify_callback(notifiable* data, notifiable::func_destroy_notify func) const;

  /** Remove a callback previously installed with add_destroy_notify_callback().
   * The callback is not executed.
   * @param data Parameter passed into previous call to add_destroy_notify_callback().
   */
  void remove_destroy_notify_callback(notifiable* data) const;

  /** Returns whether the slot is invalid.
   * @return @p true if the slot is invalid (empty).
   */
  inline bool empty() const noexcept { return (!rep_ || !rep_->call_); }

  /** Returns whether the slot is blocked.
   * @return @p true if the slot is blocked.
   */
  inline bool blocked() const noexcept { return blocked_; }

  /** Sets the blocking state.
   * If @e should_block is @p true then the blocking state is set.
   * Subsequent calls to slot::operator()() don't invoke the functor
   * contained by this slot until unblock() or block() with
   * @e should_block = @p false is called.
   * @param should_block Indicates whether the blocking state should be set or unset.
   * @return @p true if the slot was in blocking state before.
   */
  bool block(bool should_block = true) noexcept;

  /** Unsets the blocking state.
   * @return @p true if the slot was in blocking state before.
   */
  bool unblock() noexcept;

  /** Disconnects the slot.
   * Invalidates the slot and notifies the parent.
   */
  void disconnect();

  // The Tru64 and Solaris Forte 5.5 compilers needs this operator=() to be public. I'm not sure
  // why, or why it needs to be protected usually. murrayc.
  // See bug #168265.
  // protected:
  /** Overrides this slot, making a copy from another slot.
   * @param src The slot from which to make a copy.
   * @return @p this.
   */
  slot_base& operator=(const slot_base& src);

  /** Overrides this slot, making a move from another slot.
   * If @p src is connected to a parent (e.g. a signal), it is copied, not moved.
   * @param src The slot from which to move or copy.
   * @return @p this.
   */
  slot_base& operator=(slot_base&& src);

public: // public to avoid template friend declarations
  /** Typed slot_rep object that contains a functor. */
  mutable rep_type* rep_;

  /** Indicates whether the slot is blocked. */
  bool blocked_;

private:
  void delete_rep_with_check();
};

} // namespace sigc

#endif //SIGC_SLOT_BASE_HPP
