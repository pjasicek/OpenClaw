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
#ifndef SIGC_CONNECTION_HPP
#define SIGC_CONNECTION_HPP
#include <sigc++config.h>
#include <sigc++/functors/slot_base.h>
#include <sigc++/weak_raw_ptr.h>

namespace sigc
{

/** This may be used to disconnect the referred slot at any time (disconnect()).
 * If the slot has already been destroyed, disconnect() does nothing. empty() or
 * operator bool() can be used to test whether the connection is
 * still active. The connection can be blocked (block(), unblock()).
 *
 * @ingroup signal
 */
struct SIGC_API connection
{
  /** Constructs an empty connection object. */
  connection() noexcept;

  /** Constructs a connection object copying an existing one.
   * @param c The connection object to make a copy from.
   */
  connection(const connection& c);

  /** Constructs a connection object from a slot object.
   * @param slot The slot to operate on.
   */
  explicit connection(slot_base& slot);

  /** Overrides this connection object copying another one.
   * @param src The connection object to make a copy from.
   */
  connection& operator=(const connection& src);

  ~connection();

  /** Returns whether the connection is still active.
   * @return @p false if the connection is still active.
   */
  bool empty() const noexcept;

  /** Returns whether the connection is still active.
   * @return @p true if the connection is still active.
   */
  bool connected() const noexcept;

  /** Returns whether the connection is blocked.
   * @return @p true if the connection is blocked.
   */
  bool blocked() const noexcept;

  /** Sets or unsets the blocking state of this connection.
   * See slot_base::block() for details.
   * @param should_block Indicates whether the blocking state should be set or unset.
   * @return @p true if the connection has been in blocking state before.
   */
  bool block(bool should_block = true) noexcept;

  /** Unsets the blocking state of this connection.
   * @return @p true if the connection has been in blocking state before.
   */
  bool unblock() noexcept;

  /// Disconnects the referred slot.
  void disconnect();

  /** Returns whether the connection is still active.
   * @return @p true if the connection is still active.
   */
  explicit operator bool() const noexcept;

private:
  void set_slot(const sigc::internal::weak_raw_ptr<slot_base>& sl);

  /* Referred slot. Set to nullptr when the referred slot is deleted.
   * A value of nullptr indicates an "empty" connection.
   */
  sigc::internal::weak_raw_ptr<slot_base> slot_;
};

} /* namespace sigc */

#endif /* SIGC_CONNECTION_HPP */
