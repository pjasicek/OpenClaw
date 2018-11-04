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
#ifndef SIGC_MEMBER_METHOD_TRAITS_H
#define SIGC_MEMBER_METHOD_TRAITS_H

#include <sigc++config.h>

namespace sigc
{

namespace internal
{

template <typename>
struct member_method_is_const;

template <typename T_obj, typename T_result, typename... T_arg>
struct member_method_is_const<T_result (T_obj::*)(T_arg...)>
{
  constexpr static bool value = false;
};

template <typename T_obj, typename T_result, typename... T_arg>
struct member_method_is_const<T_result (T_obj::*)(T_arg...) volatile>
{
  constexpr static bool value = false;
};

template <typename T_obj, typename T_result, typename... T_arg>
struct member_method_is_const<T_result (T_obj::*)(T_arg...) const>
{
  constexpr static bool value = true;
};

template <typename T_obj, typename T_result, typename... T_arg>
struct member_method_is_const<T_result (T_obj::*)(T_arg...) const volatile>
{
  constexpr static bool value = true;
};

template <typename>
struct member_method_is_volatile;

template <typename T_obj, typename T_result, typename... T_arg>
struct member_method_is_volatile<T_result (T_obj::*)(T_arg...)>
{
  constexpr static bool value = false;
};

template <typename T_obj, typename T_result, typename... T_arg>
struct member_method_is_volatile<T_result (T_obj::*)(T_arg...) const>
{
  constexpr static bool value = false;
};

template <typename T_obj, typename T_result, typename... T_arg>
struct member_method_is_volatile<T_result (T_obj::*)(T_arg...) volatile>
{
  constexpr static bool value = true;
};

template <typename T_obj, typename T_result, typename... T_arg>
struct member_method_is_volatile<T_result (T_obj::*)(T_arg...) const volatile>
{
  constexpr static bool value = true;
};

// member method class:

template <typename T_result, typename... T_arg>
struct member_method_class
{
};

template <typename T_obj, typename T_result, typename... T_arg>
struct member_method_class<T_result (T_obj::*)(T_arg...)>
{
  using type = T_obj;
};

template <typename T_obj, typename T_result, typename... T_arg>
struct member_method_class<T_result (T_obj::*)(T_arg...) volatile>
{
  using type = T_obj;
};

template <typename T_obj, typename T_result, typename... T_arg>
struct member_method_class<T_result (T_obj::*)(T_arg...) const>
{
  using type = T_obj;
};

template <typename T_obj, typename T_result, typename... T_arg>
struct member_method_class<T_result (T_obj::*)(T_arg...) const volatile>
{
  using type = T_obj;
};

// member method result:

template <typename T_result, typename... T_arg>
struct member_method_result
{
};

template <typename T_obj, typename T_result, typename... T_arg>
struct member_method_result<T_result (T_obj::*)(T_arg...)>
{
  using type = T_result;
};

template <typename T_obj, typename T_result, typename... T_arg>
struct member_method_result<T_result (T_obj::*)(T_arg...) volatile>
{
  using type = T_result;
};

template <typename T_obj, typename T_result, typename... T_arg>
struct member_method_result<T_result (T_obj::*)(T_arg...) const>
{
  using type = T_result;
};

template <typename T_obj, typename T_result, typename... T_arg>
struct member_method_result<T_result (T_obj::*)(T_arg...) const volatile>
{
  using type = T_result;
};

} // namespace internal

} // namespace sigc

#endif /* SIGC_MEMBER_METHOD_TRAITS_H */
