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
#include <sigc++/signal_base.h>
#include <memory> // std::unique_ptr

namespace sigc
{
namespace internal
{

// Data sent from signal_impl::insert() to slot_rep::set_parent() when a slot is
// connected, and then sent from slot_rep::disconnect() to
// signal_impl::notify_self_and_iter_of_invalidated_slot()
// when the slot is disconnected. Bug 167714.
struct self_and_iter : public notifiable
{
  const std::weak_ptr<signal_impl> self_;
  const signal_impl::iterator_type iter_;

  self_and_iter(const std::weak_ptr<signal_impl>& self, const signal_impl::iterator_type& iter)
  : self_(self), iter_(iter) {}
};

signal_impl::signal_impl() : exec_count_(0), deferred_(false)
{
}

signal_impl::~signal_impl()
{
  // Disconnect all slots before *this is deleted.
  clear();
}

// only MSVC needs this to guarantee that all new/delete are executed from the DLL module
#ifdef SIGC_NEW_DELETE_IN_LIBRARY_ONLY
void*
signal_impl::operator new(size_t size_)
{
  return malloc(size_);
}

void
signal_impl::operator delete(void* p)
{
  free(p);
}
#endif

void
signal_impl::clear()
{
  // Don't let signal_impl::notify_self_and_iter_of_invalidated_slot() erase the slots.
  // It would invalidate the iterator in the following loop.
  // Don't call shared_from_this() here. clear() is called from the destructor.
  // When the destructor is executing, shared_ptr's use_count has reached 0,
  // and it's no longer possible to get a shared_ptr to this.
  const bool during_signal_emission = exec_count_ > 0;
  const bool saved_deferred = deferred_;
  signal_impl_exec_holder exec(this);

  // Disconnect all connected slots before they are deleted.
  // signal_impl::notify_self_and_iter_of_invalidated_slot() will be called and
  // delete the self_and_iter structs.
  for (auto& slot : slots_)
    slot.disconnect();

  // Don't clear slots_ during signal emission. Provided deferred_ is true,
  // sweep() will be called from ~signal_impl_holder() after signal emission,
  // and it will erase all disconnected slots.
  // https://bugzilla.gnome.org/show_bug.cgi?id=784550
  if (!during_signal_emission)
  {
    deferred_ = saved_deferred;
    slots_.clear();
  }
}

signal_impl::size_type
signal_impl::size() const noexcept
{
  return slots_.size();
}

bool
signal_impl::blocked() const noexcept
{
  for (const auto& slot : const_cast<const std::list<slot_base>&>(slots_))
  {
    if (!slot.blocked())
      return false;
  }
  return true;
}

void
signal_impl::block(bool should_block) noexcept
{
  for (auto& slot : slots_)
  {
    slot.block(should_block);
  }
}

signal_impl::iterator_type
signal_impl::connect(const slot_base& slot_)
{
  return insert(slots_.end(), slot_);
}

signal_impl::iterator_type
signal_impl::connect(slot_base&& slot_)
{
  return insert(slots_.end(), std::move(slot_));
}

void
signal_impl::add_notification_to_iter(const signal_impl::iterator_type& iter)
{
  auto si = new self_and_iter(shared_from_this(), iter);
  iter->set_parent(si, &signal_impl::notify_self_and_iter_of_invalidated_slot);
}

signal_impl::iterator_type
signal_impl::insert(signal_impl::iterator_type i, const slot_base& slot_)
{
  auto iter = slots_.insert(i, slot_);
  add_notification_to_iter(iter);
  return iter;
}

signal_impl::iterator_type
signal_impl::insert(signal_impl::iterator_type i, slot_base&& slot_)
{
  auto iter = slots_.insert(i, std::move(slot_));
  add_notification_to_iter(iter);
  return iter;
}

void
signal_impl::sweep()
{
  // The deletion of a slot may cause the deletion of a signal_base,
  // a decrementation of ref_count_, and the deletion of this.
  // In that case, the deletion of this is deferred to ~signal_impl_holder().
  signal_impl_holder exec(shared_from_this());

  deferred_ = false;
  auto i = slots_.begin();
  while (i != slots_.end())
  {
    if ((*i).empty())
      i = slots_.erase(i);
    else
      ++i;
  }
}

// static
void
signal_impl::notify_self_and_iter_of_invalidated_slot(notifiable* d)
{
  std::unique_ptr<self_and_iter> si(static_cast<self_and_iter*>(d));
  auto self = si->self_.lock();
  if (!self)
  {
    // The signal_impl object is being deleted. The use_count has reached 0.
    // Nothing to do here. exec_count_ > 0 and clear() will restore deferred_.
    return;
  }

  if (self->exec_count_ == 0)
  {
    // The deletion of a slot may cause the deletion of a signal_base,
    // a decrementation of si->self_->ref_count_, and the deletion of si->self_.
    // In that case, the deletion of si->self_ is deferred to ~signal_impl_holder().
    // https://bugzilla.gnome.org/show_bug.cgi?id=564005#c24
    signal_impl_holder exec(self);
    self->slots_.erase(si->iter_);
  }
  else
  {
    // This is occurring during signal emission or slot erasure.
    // => sweep() will be called from ~signal_impl_holder() after signal emission.
    // This is safer because we don't have to care about our
    // iterators in emit() and clear().
    self->deferred_ = true;
  }
}

} /* namespace internal */

signal_base::signal_base() noexcept
{
}

signal_base::signal_base(const signal_base& src) noexcept : impl_(src.impl())
{
}

signal_base::signal_base(signal_base&& src) : impl_(std::move(src.impl_))
{
  src.impl_ = nullptr;
}

signal_base::~signal_base()
{
}

void
signal_base::clear()
{
  if (impl_)
    impl_->clear();
}

signal_base::size_type
signal_base::size() const noexcept
{
  return (impl_ ? impl_->size() : 0);
}

bool
signal_base::blocked() const noexcept
{
  return (impl_ ? impl_->blocked() : true);
}

void
signal_base::block(bool should_block) noexcept
{
  if (impl_)
    impl_->block(should_block);
}

void
signal_base::unblock() noexcept
{
  if (impl_)
    impl_->block(false);
}

signal_base::iterator_type
signal_base::connect(const slot_base& slot_)
{
  return impl()->connect(slot_);
}

signal_base::iterator_type
signal_base::connect(slot_base&& slot_)
{
  return impl()->connect(std::move(slot_));
}

signal_base::iterator_type
signal_base::insert(iterator_type i, const slot_base& slot_)
{
  return impl()->insert(i, slot_);
}

signal_base::iterator_type
signal_base::insert(iterator_type i, slot_base&& slot_)
{
  return impl()->insert(i, std::move(slot_));
}

signal_base&
signal_base::operator=(const signal_base& src)
{
  if (src.impl_ == impl_)
    return *this;

  impl_ = src.impl();
  return *this;
}

signal_base&
signal_base::operator=(signal_base&& src)
{
  if (src.impl_ == impl_)
    return *this;

  impl_ = src.impl_;
  src.impl_ = nullptr;

  return *this;
}

std::shared_ptr<internal::signal_impl>
signal_base::impl() const
{
  if (!impl_)
  {
    impl_ = std::make_shared<internal::signal_impl>();
  }
  return impl_;
}

} /* sigc */
