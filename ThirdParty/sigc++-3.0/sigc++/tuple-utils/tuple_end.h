/* Copyright (C) 2016 Murray Cumming
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3.0 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef SIGC_TUPLE_UTILS_TUPLE_END_H
#define SIGC_TUPLE_UTILS_TUPLE_END_H

#include <sigc++/tuple-utils/tuple_cdr.h>

namespace sigc
{

namespace internal
{

namespace detail
{

template <typename T, std::size_t remove_from_start>
struct tuple_end_impl
{
  constexpr static decltype(auto) // typename tuple_type_end<T, size - remove_from_start>::type
    tuple_end(T&& t)
  {
    static_assert(remove_from_start > 0, "remove_from_start must be more than zero.");

    using cdr = typename tuple_type_cdr<std::decay_t<T>>::type;
    return tuple_end_impl<cdr, remove_from_start - 1>::tuple_end(tuple_cdr(std::forward<T>(t)));
  }
};

template <typename T>
struct tuple_end_impl<T, 1>
{
  constexpr static decltype(auto) tuple_end(T&& t) { return tuple_cdr(std::forward<T>(t)); }
};

template <typename T>
struct tuple_end_impl<T, 0>
{
  constexpr static decltype(auto) tuple_end(T&& t) { return std::forward<T>(t); }
};

} // detail namespace

/**
 * Get the tuple with the last @a len items of the original.
 */
template <std::size_t len, typename T>
constexpr decltype(auto) // typename tuple_type_end<T, len>::type
  tuple_end(T&& t)
{
  // We use std::decay_t<> because tuple_size is not defined for references.
  constexpr auto size = std::tuple_size<std::decay_t<T>>::value;
  static_assert(len <= size, "The tuple size must be less than or equal to the length.");
  constexpr auto size_start = size - len;
  return detail::tuple_end_impl<T, size_start>::tuple_end(std::forward<T>(t));
}

} // namespace internal

} // namespace sigc

#endif // SIGC_TUPLE_UTILS_TUPLE_END_H
