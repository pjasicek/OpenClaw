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

#include <sigc++/functors/slot_base.h>
#include <sigc++/weak_raw_ptr.h>

namespace
{
// Used by slot_base::set_parent() when a slot_base without a rep_ is assigned a parent.
class dummy_slot_rep : public sigc::internal::slot_rep
{
public:
  dummy_slot_rep() : slot_rep(nullptr) {}
  sigc::internal::slot_rep* clone() const override { return new dummy_slot_rep(); }
  void destroy() override {}
};
} // anonymous namespace

namespace sigc
{
namespace internal
{
// only MSVC needs this to guarantee that all new/delete are executed from the DLL module
#ifdef SIGC_NEW_DELETE_IN_LIBRARY_ONLY
void*
slot_rep::operator new(size_t size_)
{
  return malloc(size_);
}

void
slot_rep::operator delete(void* p)
{
  free(p);
}
#endif

void
slot_rep::disconnect()
{
  // Invalidate the slot.
  // _Must_ be done here because parent_ might defer the actual
  // destruction of the slot_rep and try to invoke it before that point.
  // Must be done also for a slot without a parent, according to
  // https://bugzilla.gnome.org/show_bug.cgi?id=311057
  // See also https://bugzilla.gnome.org/show_bug.cgi?id=738602
  call_ = nullptr;

  if (parent_)
  {
    auto data_ = parent_;
    parent_ = nullptr; // Just a precaution.
    (cleanup_)(data_); // Notify the parent (might lead to destruction of this!).
  }
}

// static
void
slot_rep::notify_slot_rep_invalidated(notifiable* data)
{
  auto self_ = static_cast<slot_rep*>(data);

  self_->call_ = nullptr; // Invalidate the slot.

  // Make sure we are notified if disconnect() deletes self_, which is trackable.
  sigc::internal::weak_raw_ptr<slot_rep> notifier(self_);
  self_->disconnect(); // Disconnect the slot (might lead to deletion of self_!).
  // If self_ has been deleted, then the weak_raw_ptr will have been invalidated.
  if (notifier)
  {
    // Detach the stored functor from the other referred trackables and destroy it.
    // destroy() might lead to deletion of self_. Bug #564005.
    self_->destroy();
  }
}

} // namespace internal

slot_base::slot_base() noexcept : rep_(nullptr), blocked_(false)
{
}

slot_base::slot_base(rep_type* rep) noexcept : rep_(rep), blocked_(false)
{
}

slot_base::slot_base(const slot_base& src) : rep_(nullptr), blocked_(src.blocked_)
{
  if (src.rep_)
  {
    // Check call_ so we can ignore invalidated slots.
    // Otherwise, destroyed bound reference parameters (whose destruction caused the slot's
    // invalidation) may be used during clone().
    // Note: I'd prefer to check somewhere during clone(). murrayc.
    if (src.rep_->call_)
      rep_ = src.rep_->clone();
    else
    {
      *this = slot_base(); // Return the default invalid slot.
    }
  }
}

slot_base::slot_base(slot_base&& src) : rep_(nullptr), blocked_(src.blocked_)
{
  if (src.rep_)
  {
    if (src.rep_->parent_)
    {
      // src is connected to a parent, e.g. a sigc::signal.
      // Copy, don't move! See https://bugzilla.gnome.org/show_bug.cgi?id=756484

      // Check call_ so we can ignore invalidated slots.
      // Otherwise, destroyed bound reference parameters (whose destruction
      // caused the slot's invalidation) may be used during clone().
      if (src.rep_->call_)
        rep_ = src.rep_->clone();
      else
        blocked_ = false; // Return the default invalid slot.
    }
    else
    {
      // src is not connected. Really move src.rep_.
      src.rep_->notify_callbacks();
      rep_ = src.rep_;

      // Wipe src:
      src.rep_ = nullptr;
      src.blocked_ = false;
    }
  }
}

slot_base::~slot_base()
{
  delete rep_;
}

slot_base::operator bool() const noexcept
{
  return rep_ != nullptr;
}

void
slot_base::delete_rep_with_check()
{
  if (!rep_)
    return;

  // Make sure we are notified if disconnect() deletes rep_, which is trackable.
  // Compare slot_rep::notify_slot_rep_invalidated().
  sigc::internal::weak_raw_ptr<rep_type> notifier(rep_);
  rep_->disconnect(); // Disconnect the slot (might lead to deletion of rep_!).

  // If rep_ has been deleted, don't try to delete it again.
  // If it has been deleted, this slot_base has probably also been deleted, so
  // don't clear the rep_ pointer. It's the responsibility of the code that
  // deletes rep_ to either clear the rep_ pointer or delete this slot_base.
  if (notifier)
  {
    delete rep_; // Detach the stored functor from the other referred trackables and destroy it.
    rep_ = nullptr;
  }
}

slot_base&
slot_base::operator=(const slot_base& src)
{
  if (src.rep_ == rep_)
  {
    blocked_ = src.blocked_;
    return *this;
  }

  if (src.empty())
  {
    delete_rep_with_check();

    return *this;
  }

  auto new_rep_ = src.rep_->clone();

  if (rep_) // Silently exchange the slot_rep.
  {
    new_rep_->set_parent(rep_->parent_, rep_->cleanup_);
    delete rep_; // Calls destroy(), but does not call disconnect().
  }

  rep_ = new_rep_;
  blocked_ = src.blocked_;

  return *this;
}

slot_base&
slot_base::operator=(slot_base&& src)
{
  if (src.rep_ == rep_)
  {
    blocked_ = src.blocked_;
    return *this;
  }

  if (src.empty())
  {
    delete_rep_with_check();
    return *this;
  }

  blocked_ = src.blocked_;
  internal::slot_rep* new_rep_ = nullptr;
  if (src.rep_->parent_)
  {
    // src is connected to a parent, e.g. a sigc::signal.
    // Copy, don't move! See https://bugzilla.gnome.org/show_bug.cgi?id=756484
    new_rep_ = src.rep_->clone();
  }
  else
  {
    // src is not connected. Really move src.rep_.
    src.rep_->notify_callbacks();
    new_rep_ = src.rep_;

    // Wipe src:
    src.rep_ = nullptr;
    src.blocked_ = false;
  }

  if (rep_) // Silently exchange the slot_rep.
  {
    new_rep_->set_parent(rep_->parent_, rep_->cleanup_);
    delete rep_; // Calls destroy(), but does not call disconnect().
  }
  rep_ = new_rep_;
  return *this;
}

void
slot_base::set_parent(notifiable* parent, notifiable::func_destroy_notify cleanup) const noexcept
{
  if (!rep_)
    rep_ = new dummy_slot_rep();
  rep_->set_parent(parent, cleanup);
}

void
slot_base::add_destroy_notify_callback(notifiable* data, func_destroy_notify func) const
{
  if (rep_)
    rep_->add_destroy_notify_callback(data, func);
}

void
slot_base::remove_destroy_notify_callback(notifiable* data) const
{
  if (rep_)
    rep_->remove_destroy_notify_callback(data);
}

bool
slot_base::block(bool should_block) noexcept
{
  bool old = blocked_;
  blocked_ = should_block;
  return old;
}

bool
slot_base::unblock() noexcept
{
  return block(false);
}

void
slot_base::disconnect()
{
  if (rep_)
    rep_->disconnect();
}

/*bool slot_base::empty() const // having this function not inline is killing performance !!!
{
  if (rep_ && !rep_->call_)
    {
      delete rep_;        // This is not strictly necessary here. I'm convinced that it is
      rep_ = nullptr;           // safe to wait for the destructor to delete the slot_rep. Martin.
    }
  return (rep_ == nullptr);
}*/

} // namespace sigc
