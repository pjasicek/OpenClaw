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

#ifndef SIGC_TUPLE_UTILS_TUPLE_START_H
#define SIGC_TUPLE_UTILS_TUPLE_START_H

#include <tuple>
#include <utility>

namespace sigc
{

namespace internal
{

namespace detail
{

template <typename T, typename Seq>
struct tuple_type_start_impl;

template <typename T, std::size_t... I>
struct tuple_type_start_impl<T, std::index_sequence<I...>>
{
  using type = std::tuple<typename std::tuple_element<I, T>::type...>;
};

} // detail namespace

/**
 * Get the type of a tuple with just the first @len items.
 */
template <typename T, std::size_t len>
struct tuple_type_start : detail::tuple_type_start_impl<T, std::make_index_sequence<len>>
{
};

namespace detail
{

template <typename T, typename Seq>
struct tuple_start_impl;

template <typename T, std::size_t... I>
struct tuple_start_impl<T, std::index_sequence<I...>>
{
  static constexpr decltype(auto) tuple_start(T&& t)
  {
    constexpr auto size = std::tuple_size<std::decay_t<T>>::value;
    constexpr auto len = sizeof...(I);
    static_assert(len <= size, "The tuple size must be less than or equal to the length.");

    using start = typename tuple_type_start<std::decay_t<T>, len>::type;
    return start(std::get<I>(std::forward<T>(t))...);
  }
};

} // detail namespace

/**
 * Get the tuple with the last @a len items of the original.
 */
template <std::size_t len, typename T>
constexpr decltype(auto) // typename tuple_type_end<T, len>::type
  tuple_start(T&& t)
{
  // We use std::decay_t<> because tuple_size is not defined for references.
  constexpr auto size = std::tuple_size<std::decay_t<T>>::value;
  static_assert(len <= size, "The tuple size must be less than or equal to the length.");

  return detail::tuple_start_impl<T, std::make_index_sequence<len>>::tuple_start(
    std::forward<T>(t));
}

} // namespace internal

} // namespace sigc

#endif // SIGC_TUPLE_UTILS_TUPLE_START_H
