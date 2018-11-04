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

#ifndef SIGC_TUPLE_UTILS_TUPLE_FOR_EACH_H
#define SIGC_TUPLE_UTILS_TUPLE_FOR_EACH_H

#include <tuple>

namespace sigc
{

namespace internal
{

namespace detail
{

template <template <typename> class T_visitor, std::size_t size_from_index, typename... T_extras>
struct tuple_for_each_impl
{
  template <typename T>
  constexpr static void tuple_for_each(T&& t, T_extras&&... extras)
  {
    // We use std::decay_t<> because tuple_size is not defined for references.
    constexpr auto size = std::tuple_size<std::decay_t<T>>::value;
    static_assert(size > 1, "size must be more than 0.");

    constexpr auto index = size - size_from_index;
    static_assert(index >= 0, "unexpected index.");

    using element_type = typename std::tuple_element<index, std::decay_t<T>>::type;
    T_visitor<element_type>::visit(std::get<index>(t), std::forward<T_extras>(extras)...);

    tuple_for_each_impl<T_visitor, size_from_index - 1, T_extras...>::tuple_for_each(
      std::forward<T>(t), std::forward<T_extras>(extras)...);
  }
};

template <template <typename> class T_visitor, typename... T_extras>
struct tuple_for_each_impl<T_visitor, 1, T_extras...>
{
  template <typename T>
  constexpr static void tuple_for_each(T&& t, T_extras&&... extras)
  {
    // We use std::decay_t<> because tuple_size is not defined for references.
    constexpr auto size = std::tuple_size<std::decay_t<T>>::value;
    static_assert(size > 0, "size must be more than 0.");

    constexpr auto index = size - 1;
    static_assert(index >= 0, "unexpected index.");

    using element_type = typename std::tuple_element<index, std::decay_t<T>>::type;
    T_visitor<element_type>::visit(
      std::get<index>(std::forward<T>(t)), std::forward<T_extras>(extras)...);
  }
};

template <template <typename> class T_visitor, typename... T_extras>
struct tuple_for_each_impl<T_visitor, 0, T_extras...>
{
  template <typename T>
  constexpr static void tuple_for_each(T&& /* t */, T_extras&&... /* extras */)
  {
    // Do nothing because the tuple has no elements.
  }
};

} // detail namespace

/**
 * Call the @e T_Visitor functors visit() method for each element,
 * from the first to the last.
 *
 * @tparam T_visitor should be a template that has a static visit() method.
 * @tparam T the tuple type.
 * @tparam T_extras the types of any extra arguments to pass to @e T_Visitor's
 * visit() method.
 * @param t The tuple whose elements should be visited.
 * @param extras Any extra arguments to pass to @e T_Visitor's visit() method.
 */
template <template <typename> class T_visitor, typename T, typename... T_extras>
constexpr void
tuple_for_each(T&& t, T_extras&&... extras)
{
  // We use std::decay_t<> because tuple_size is not defined for references.
  constexpr auto size = std::tuple_size<std::decay_t<T>>::value;

  if (size == 0)
  {
    return;
  }

  detail::tuple_for_each_impl<T_visitor, size, T_extras...>::tuple_for_each(
    std::forward<T>(t), std::forward<T_extras>(extras)...);
}

} // namespace internal

} // namespace sigc

#endif // SIGC_TUPLE_UTILS_TUPLE_FOR_EACH_H
