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
 *
 */

#include <sigc++/trackable.h>

namespace sigc
{

trackable::trackable() noexcept : callback_list_(nullptr)
{
}

/* Don't copy the notification list.
   The objects watching src don't need to be notified when the new object dies. */
trackable::trackable(const trackable& /*src*/) noexcept : callback_list_(nullptr)
{
}

// Don't move the notification list.
// The objects watching src don't need to be notified when the new object dies.
// They need to be notified now, because src probably becomes useless.
//
// If trackable's move constructor is modified, check if Glib::Object's
// move constructor should be modified similarly.
trackable::trackable(trackable&& src) noexcept
: callback_list_(nullptr)
{
  src.notify_callbacks();
}

trackable&
trackable::operator=(const trackable& src)
{
  if (this != &src)
    notify_callbacks(); // Make sure that we have finished with existing stuff before replacing it.

  return *this;
}

trackable&
trackable::operator=(trackable&& src) noexcept
{
  if (this != &src)
  {
    notify_callbacks(); // Make sure that we have finished with existing stuff before replacing it.
    src.notify_callbacks(); // src probably becomes useless.
  }
  return *this;
}

trackable::~trackable()
{
  notify_callbacks();
}

void
trackable::add_destroy_notify_callback(notifiable* data, func_destroy_notify func) const
{
  callback_list()->add_callback(data, func);
}

void
trackable::remove_destroy_notify_callback(notifiable* data) const
{
  callback_list()->remove_callback(data);
}

void
trackable::notify_callbacks()
{
  delete callback_list_; // This invokes all of the callbacks.
  callback_list_ = nullptr;
}

internal::trackable_callback_list*
trackable::callback_list() const
{
  if (!callback_list_)
    callback_list_ = new internal::trackable_callback_list;

  return callback_list_;
}

namespace internal
{

trackable_callback_list::~trackable_callback_list()
{
  clearing_ = true;

  for (auto& callback : callbacks_)
  {
    if (callback.func_)
      callback.func_(callback.data_);
  }
}

void
trackable_callback_list::add_callback(notifiable* data, func_destroy_notify func)
{
  // TODO: Is it okay to silently ignore attempts to add dependencies when the list
  // is being cleared?
  // I'd consider this a serious application bug, since the app is likely to segfault.
  // But then, how should we handle it? Throw an exception? Martin.
  if (!clearing_)
    callbacks_.emplace_back(trackable_callback(data, func));
}

void
trackable_callback_list::clear()
{
  clearing_ = true;

  for (auto& callback : callbacks_)
  {
    if (callback.func_)
      callback.func_(callback.data_);
  }

  callbacks_.clear();

  clearing_ = false;
}

void
trackable_callback_list::remove_callback(notifiable* data)
{
  for (auto i = callbacks_.begin(); i != callbacks_.end(); ++i)
  {
    auto& callback = *i;
    if (callback.data_ == data && callback.func_ != nullptr)
    {
      // Don't remove a list element while the list is being cleared.
      // It could invalidate the iterator in ~trackable_callback_list() or clear().
      // But it may be necessary to invalidate the callback. See bug 589202.
      if (clearing_)
        callback.func_ = nullptr;
      else
        callbacks_.erase(i);
      return;
    }
  }
}

} /* namespace internal */

} /* namespace sigc */
