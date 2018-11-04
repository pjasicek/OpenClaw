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

#include <sigc++/connection.h>

namespace sigc
{

connection::connection() noexcept : slot_(nullptr)
{
}

connection::connection(slot_base& slot)
: slot_(&slot)
{
}


connection::connection(const connection& c) : slot_(c.slot_)
{
}

connection&
connection::operator=(const connection& src)
{
  set_slot(src.slot_);
  return *this;
}

connection::~connection()
{
}

bool
connection::empty() const noexcept
{
  return (!slot_ || slot_->empty());
}

bool
connection::connected() const noexcept
{
  return !empty();
}

bool
connection::blocked() const noexcept
{
  return (slot_ ? slot_->blocked() : false);
}

bool
connection::block(bool should_block) noexcept
{
  return (slot_ ? slot_->block(should_block) : false);
}

bool
connection::unblock() noexcept
{
  return (slot_ ? slot_->unblock() : false);
}

void
connection::disconnect()
{
  if (slot_)
    slot_->disconnect(); // This notifies slot_'s parent.
}

connection::operator bool() const noexcept
{
  return !empty();
}

void
connection::set_slot(const sigc::internal::weak_raw_ptr<slot_base>& sl)
{
  slot_ = sl;
}


} /* namespace sigc */
