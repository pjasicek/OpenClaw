/*
 * Copyright 2002 - 2016, The libsigc++ Development Team
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

#ifndef SIGC_SIGNAL_BASE_H
#define SIGC_SIGNAL_BASE_H

#include <cstddef>
#include <list>
#include <memory> //For std::shared_ptr<>
#include <sigc++config.h>
#include <sigc++/type_traits.h>
#include <sigc++/functors/slot.h>
#include <sigc++/functors/mem_fun.h>

/** The libsigc++ namespace.
 */
namespace sigc
{

namespace internal
{

/** Implementation of the signal interface.
 * signal_impl manages a list of slots. When a slot becomes invalid (because some
 * referred object dies), notify_self_and_iter_of_invalidated_slot() is executed.
 * notify_self_and_iter_of_invalidated_slot() either calls slots_.erase() directly
 * or defers the execution of erase() to sweep() when the signal is being emitted.
 * sweep() removes all invalid slots from the list.
 */
struct SIGC_API signal_impl
 : public std::enable_shared_from_this<signal_impl>
{
  using size_type = std::size_t;
  using slot_list = std::list<slot_base>;
  using iterator_type = slot_list::iterator;
  using const_iterator_type = slot_list::const_iterator;

  signal_impl();
  ~signal_impl();

  signal_impl(const signal_impl& src) = delete;
  signal_impl& operator=(const signal_impl& src) = delete;

  signal_impl(signal_impl&& src) = delete;
  signal_impl& operator=(signal_impl&& src) = delete;

// only MSVC needs this to guarantee that all new/delete are executed from the DLL module
#ifdef SIGC_NEW_DELETE_IN_LIBRARY_ONLY
  void* operator new(size_t size_);
  void operator delete(void* p);
#endif

  /// Increments the reference and execution counter.
  inline void reference_exec() noexcept
  {
    ++exec_count_;
  }

  /** Decrements the reference and execution counter.
   * Invokes sweep() if the execution counter reaches zero and the
   * removal of one or more slots has been deferred.
   */
  inline void unreference_exec()
  {
    if (!(--exec_count_) && deferred_)
      sweep();
  }

  /** Returns whether the list of slots is empty.
   * @return @p true if the list of slots is empty.
   */
  inline bool empty() const noexcept { return slots_.empty(); }

  /// Empties the list of slots.
  void clear();

  /** Returns the number of slots in the list.
   * @return The number of slots in the list.
   */
  size_type size() const noexcept;

  /** Returns whether all slots in the list are blocked.
   * @return @p true if all slots are blocked or the list is empty.
   *
   * @newin{2,4}
   */
  bool blocked() const noexcept;

  /** Sets the blocking state of all slots in the list.
   * If @e should_block is @p true then the blocking state is set.
   * Subsequent emissions of the signal don't invoke the functors
   * contained in the slots until block() with @e should_block = @p false is called.
   * sigc::slot_base::block() and sigc::slot_base::unblock() can change the
   * blocking state of individual slots.
   * @param should_block Indicates whether the blocking state should be set or unset.
   *
   * @newin{2,4}
   */
  void block(bool should_block = true) noexcept;

  /** Adds a slot at the bottom of the list of slots.
   * @param slot_ The slot to add to the list of slots.
   * @return An iterator pointing to the new slot in the list.
   */
  iterator_type connect(const slot_base& slot_);

  /** Adds a slot at the bottom of the list of slots.
   * @param slot_ The slot to add to the list of slots.
   * @return An iterator pointing to the new slot in the list.
   *
   * @newin{2,8}
   */
  iterator_type connect(slot_base&& slot_);

  /** Adds a slot at the given position into the list of slots.
   * @param i An iterator indicating the position where @p slot_ should be inserted.
   * @param slot_ The slot to add to the list of slots.
   * @return An iterator pointing to the new slot in the list.
   */
  iterator_type insert(iterator_type i, const slot_base& slot_);

  /** Adds a slot at the given position into the list of slots.
   * @param i An iterator indicating the position where @p slot_ should be inserted.
   * @param slot_ The slot to add to the list of slots.
   * @return An iterator pointing to the new slot in the list.
   *
   * @newin{2,8}
   */
  iterator_type insert(iterator_type i, slot_base&& slot_);

  /// Removes invalid slots from the list of slots.
  void sweep();

private:
  /** Callback that is executed when some slot becomes invalid.
   * This callback is registered in every slot when inserted into
   * the list of slots. It is executed when a slot becomes invalid
   * because of some referred object being destroyed.
   * It either calls slots_.erase() directly or defers the execution of
   * erase() to sweep() when the signal is being emitted.
   * @param d A local structure, created in insert().
   */
  static void notify_self_and_iter_of_invalidated_slot(notifiable* d);

  void add_notification_to_iter(const signal_impl::iterator_type& iter);

public:
  /// The list of slots.
  std::list<slot_base> slots_;

private:
  /** Execution counter.
   * Indicates whether the signal is being emitted.
   */
  short exec_count_;

  /// Indicates whether the execution of sweep() is being deferred.
  bool deferred_;
};

struct SIGC_API signal_impl_exec_holder
{
  /** Increments the execution counter of the parent sigc::signal_impl object.
   * @param sig The parent sigc::signal_impl object.
   */
  inline explicit signal_impl_exec_holder(signal_impl* sig) noexcept
  : sig_(sig)
  {
    sig_->reference_exec();
  }

  signal_impl_exec_holder(const signal_impl_exec_holder& src) = delete;
  signal_impl_exec_holder operator=(const signal_impl_exec_holder& src) = delete;

  signal_impl_exec_holder(signal_impl_exec_holder&& src) = delete;
  signal_impl_exec_holder operator=(signal_impl_exec_holder&& src) = delete;

  /// Decrements the reference and execution counter of the parent sigc::signal_impl object.
  inline ~signal_impl_exec_holder() { sig_->unreference_exec(); }

protected:
  /// The parent sigc::signal_impl object.
  signal_impl* sig_;
};

/// Exception safe sweeper for cleaning up invalid slots on the slot list.
struct SIGC_API signal_impl_holder
{
  /** Increments the reference and execution counter of the parent sigc::signal_impl object.
   * @param sig The parent sigc::signal_impl object.
   */
  inline signal_impl_holder(const std::shared_ptr<signal_impl>& sig) noexcept
  : sig_(sig), exec_holder_(sig.get())
  {
  }

  signal_impl_holder(const signal_impl_holder& src) = delete;
  signal_impl_holder operator=(const signal_impl_holder& src) = delete;

  signal_impl_holder(signal_impl_holder&& src) = delete;
  signal_impl_holder operator=(signal_impl_holder&& src) = delete;

protected:
  /// The parent sigc::signal_impl object.
  const std::shared_ptr<signal_impl> sig_;
  signal_impl_exec_holder exec_holder_;
};

} /* namespace internal */

/** @defgroup signal Signals
 * Use sigc::signal::connect() with sigc::mem_fun() and sigc::ptr_fun() to connect a method or
 * function with a signal.
 *
 * @code
 * signal_clicked.connect( sigc::mem_fun(*this, &MyWindow::on_clicked) );
 * @endcode
 *
 * When the signal is emitted your method will be called.
 *
 * signal::connect() returns a connection, which you can later use to disconnect your method.
 * If the type of your object inherits from sigc::trackable the method is disconnected
 * automatically when your object is destroyed.
 *
 * When signals are copied they share the underlying information,
 * so you can have a protected/private sigc::signal member and a public accessor method.
 * A sigc::signal is a kind of reference-counting pointer. It's similar to
 * std::shared_ptr<>, although sigc::signal is restricted to holding a pointer to
 * a sigc::internal::signal_impl object that contains the implementation of the signal.
 *
 * @code
 * class MyClass
 * {
 * public:
 *   using MySignalType = sigc::signal<void()>;
 *   MySignalType get_my_signal() { return m_my_signal; }
 * private:
 *   MySignalType m_my_signal;
 * };
 * @endcode
 *
 * signal and slot objects provide the core functionality of this
 * library. A slot is a container for an arbitrary functor.
 * A signal is a list of slots that are executed on emission.
 * For compile time type safety a list of template arguments
 * must be provided for the signal template that determines the
 * parameter list for emission. Functors and closures are converted
 * into slots implicitly on connection, triggering compiler errors
 * if the given functor or closure cannot be invoked with the
 * parameter list of the signal to connect to.
 *
 * Almost any functor with the correct signature can be converted to a sigc::slot
 * and connected to a signal. See @ref slot "Slots" and sigc::signal::connect().
 */

/** Base class for the sigc::signal# templates.
 * signal_base integrates most of the interface of the derived sigc::signal#
 * templates. The implementation, however, resides in sigc::internal::signal_impl.
 * A sigc::internal::signal_impl object is dynamically allocated from signal_base
 * when first connecting a slot to the signal. This ensures that empty signals
 * don't waste memory.
 *
 * sigc::internal::signal_impl is reference-counted. When a sigc::signal# object
 * is copied, the reference count of its sigc::internal::signal_impl object is
 * incremented. Both sigc::signal# objects then refer to the same
 * sigc::internal::signal_impl object.
 *
 * @ingroup signal
 */
struct SIGC_API signal_base
{
  using size_type = std::size_t;

  signal_base() noexcept;

  signal_base(const signal_base& src) noexcept;

  signal_base(signal_base&& src);

  ~signal_base();

  signal_base& operator=(const signal_base& src);

  signal_base& operator=(signal_base&& src);

  /** Returns whether the list of slots is empty.
   * @return @p true if the list of slots is empty.
   */
  inline bool empty() const noexcept { return (!impl_ || impl_->empty()); }

  /// Empties the list of slots.
  void clear();

  /** Returns the number of slots in the list.
   * @return The number of slots in the list.
   */
  size_type size() const noexcept;

  /** Returns whether all slots in the list are blocked.
   * @return @p true if all slots are blocked or the list is empty.
   *
   * @newin{2,4}
   */
  bool blocked() const noexcept;

  /** Sets the blocking state of all slots in the list.
   * If @e should_block is @p true then the blocking state is set.
   * Subsequent emissions of the signal don't invoke the functors
   * contained in the slots until unblock() or block() with
   * @e should_block = @p false is called.
   * sigc::slot_base::block() and sigc::slot_base::unblock() can change the
   * blocking state of individual slots.
   * @param should_block Indicates whether the blocking state should be set or unset.
   *
   * @newin{2,4}
   */
  void block(bool should_block = true) noexcept;

  /** Unsets the blocking state of all slots in the list.
   *
   * @newin{2,4}
   */
  void unblock() noexcept;

protected:
  using iterator_type = internal::signal_impl::iterator_type;

  /** Adds a slot at the end of the list of slots.
   * With connect(), slots can also be added during signal emission.
   * In this case, they won't be executed until the next emission occurs.
   * @param slot_ The slot to add to the list of slots.
   * @return An iterator pointing to the new slot in the list.
   */
  iterator_type connect(const slot_base& slot_);

  /** Adds a slot at the end of the list of slots.
   * With connect(), slots can also be added during signal emission.
   * In this case, they won't be executed until the next emission occurs.
   * @param slot_ The slot to add to the list of slots.
   * @return An iterator pointing to the new slot in the list.
   *
   * @newin{2,8}
   */
  iterator_type connect(slot_base&& slot_);

  /** Adds a slot at the given position into the list of slots.
   * Note that this function does not work during signal emission!
   * @param i An iterator indicating the position where @e slot_ should be inserted.
   * @param slot_ The slot to add to the list of slots.
   * @return An iterator pointing to the new slot in the list.
   */
  iterator_type insert(iterator_type i, const slot_base& slot_);

  /** Adds a slot at the given position into the list of slots.
   * Note that this function does not work during signal emission!
   * @param i An iterator indicating the position where @e slot_ should be inserted.
   * @param slot_ The slot to add to the list of slots.
   * @return An iterator pointing to the new slot in the list.
   *
   * @newin{2,8}
   */
  iterator_type insert(iterator_type i, slot_base&& slot_);

  /** Returns the signal_impl object encapsulating the list of slots.
   * @return The signal_impl object encapsulating the list of slots.
   */
  std::shared_ptr<internal::signal_impl> impl() const;

  /// The signal_impl object encapsulating the slot list.
  mutable std::shared_ptr<internal::signal_impl> impl_;
};

} // namespace sigc

#endif /* SIGC_SIGNAL_BASE_H */
