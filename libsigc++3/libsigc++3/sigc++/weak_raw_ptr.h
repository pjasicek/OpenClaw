/*
 * Copyright 2016, The libsigc++ Development Team
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

#ifndef SIGC_WEAK_RAW_PTR_HPP
#define SIGC_WEAK_RAW_PTR_HPP
#include <sigc++/trackable.h> //Just for notifiable.

namespace sigc
{

struct notifiable;

namespace internal
{

/** T must derive from sigc::trackable.
 */
template <typename T>
struct weak_raw_ptr : public sigc::notifiable
{
  inline weak_raw_ptr()
  : p_(nullptr)
  {}

  inline weak_raw_ptr(T* p) noexcept
  : p_(p)
  {
    if(!p)
      return;

    p->add_destroy_notify_callback(this, &notify_object_invalidated);
  }

  inline weak_raw_ptr(const weak_raw_ptr& src) noexcept
  : p_(src.p_)
  {
    p_->add_destroy_notify_callback(this, &notify_object_invalidated);
  }

  inline weak_raw_ptr& operator=(const weak_raw_ptr& src) noexcept
  {
    if(p_) {
      p_->remove_destroy_notify_callback(this);
    }

    p_ = src.p_;
    p_->add_destroy_notify_callback(this, &notify_object_invalidated);

    return *this;
  }

  //TODO:
  weak_raw_ptr(weak_raw_ptr&& src) = delete;
  weak_raw_ptr& operator=(weak_raw_ptr&& src) = delete;

  inline ~weak_raw_ptr() noexcept
  {
    if (p_) {
      p_->remove_destroy_notify_callback(this);
    }
  }

  inline explicit operator bool() const noexcept
  {
    return p_ != nullptr;
  }

  inline T* operator->() const noexcept
  {
    return p_;
  }

private:
  /** Callback that is executed when the objet is destroyed.
   * @param data The object notified (@p this).
   */
  static void notify_object_invalidated(notifiable* data)
  {
    auto self = static_cast<weak_raw_ptr*>(data);
    if(!self)
      return;

    self->p_ = nullptr;
  }

  T* p_;
};

} /* namespace internal */

} /* namespace sigc */

#endif /* SIGC_WEAK_RAW_PTR_HPP */
