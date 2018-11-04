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

#ifndef SIGC_TUPLE_UTILS_TUPLE_TRANSFORM_EACH_H
#define SIGC_TUPLE_UTILS_TUPLE_TRANSFORM_EACH_H

#include <sigc++/tuple-utils/tuple_cdr.h>
#include <sigc++/tuple-utils/tuple_end.h>
#include <sigc++/tuple-utils/tuple_start.h>
#include <type_traits>

namespace sigc
{

namespace internal
{

namespace detail
{

template <template <typename> class T_transformer, std::size_t size_from_index>
struct tuple_transform_each_impl
{
  // TODO: Avoid the need to pass t_original all the way into the recursion?
  template <typename T_current, typename T_original>
  constexpr static decltype(auto) tuple_transform_each(T_current&& t, T_original& t_original)
  {
    // We use std::decay_t<> because tuple_size is not defined for references.
    constexpr auto size = std::tuple_size<std::decay_t<T_current>>::value;
    static_assert(size > 1, "size must be more than 0.");

    constexpr auto index = size - size_from_index;
    static_assert(index >= 0, "unexpected index.");

    using from_element_type = typename std::tuple_element<index, std::decay_t<T_original>>::type;
    using to_element_type = typename std::result_of<decltype (
      &T_transformer<from_element_type>::transform)(from_element_type&)>::type;
    const auto t_element = std::tuple<to_element_type>(
      T_transformer<from_element_type>::transform(std::get<index>(t_original)));

    const auto t_start = tuple_start<index>(std::forward<T_current>(t));

    // t_end's elements will be copies of the elements in t, so this method's
    // caller won't see the changes made in the subsequent call of
    // tuple_transform_each() on those copies. That's why we pass t_original
    // through too, so we can modify that directly.
    // the const version (tuple_transform_each_const()) doesn't have to worry
    // about this, though avoiding copying would be more efficient.
    const auto t_end = tuple_end<size - index - 1>(t);

    auto t_with_transformed_element = std::tuple_cat(t_start, t_element, t_end);
    return tuple_transform_each_impl<T_transformer, size_from_index - 1>::tuple_transform_each(
      t_with_transformed_element, t_original);
  }
};

template <template <typename> class T_transformer>
struct tuple_transform_each_impl<T_transformer, 1>
{
  template <typename T_current, typename T_original>
  constexpr static decltype(auto) tuple_transform_each(T_current&& t, T_original& t_original)
  {
    // We use std::decay_t<> because tuple_size is not defined for references.
    constexpr auto size = std::tuple_size<std::decay_t<T_current>>::value;
    static_assert(size > 0, "size must be more than 0.");

    constexpr auto index = size - 1;
    static_assert(index >= 0, "unexpected index.");

    using from_element_type = typename std::tuple_element<index, T_original>::type;
    using to_element_type = typename std::result_of<decltype (
      &T_transformer<from_element_type>::transform)(from_element_type&)>::type;
    const auto tuple_element = std::tuple<to_element_type>(
      T_transformer<from_element_type>::transform(std::get<index>(t_original)));

    const auto tuple_rest = tuple_start<size - 1>(std::forward<T_current>(t));
    return std::tuple_cat(tuple_rest, tuple_element);
  }
};

template <template <typename> class T_transformer>
struct tuple_transform_each_impl<T_transformer, 0>
{
  template <typename T_current, typename T_original>
  constexpr static decltype(auto) tuple_transform_each(T_current&& t, T_original& /* t_original */)
  {
    // Do nothing because the tuple has no elements.
    return std::forward<T_current>(t);
  }
};

} // detail namespace

/**
 * Get a tuple with each element having the transformed value of the element
 * in the original tuple.
 */
template <template <typename> class T_transformer, typename T>
constexpr decltype(auto)
tuple_transform_each(T&& t)
{
  // We use std::decay_t<> because tuple_size is not defined for references.
  constexpr auto size = std::tuple_size<std::decay_t<T>>::value;

  return detail::tuple_transform_each_impl<T_transformer, size>::tuple_transform_each(
    std::forward<T>(t), t);
}

} // namespace internal

} // namespace sigc

#endif // SIGC_TUPLE_UTILS_TUPLE_TRANSFORM_EACH_H
