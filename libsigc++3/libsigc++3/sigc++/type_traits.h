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
#ifndef SIGC_TYPE_TRAIT_H
#define SIGC_TYPE_TRAIT_H

#include <sigc++config.h>

namespace sigc
{

template <typename T_type>
struct type_trait
{
  using pass = T_type&;
  using take = const T_type&;
};

template <typename T_type, int N>
struct type_trait<T_type[N]>
{
  using pass = T_type*&;
  using take = const T_type*&;
};

template <typename T_type>
struct type_trait<T_type&>
{
  using pass = T_type&;
  using take = T_type&;
};

template <typename T_type>
struct type_trait<const T_type&>
{
  using pass = const T_type&;
  using take = const T_type&;
};

template <>
struct type_trait<void>
{
  using pass = void;
  using take = void;
};

template <typename T>
using type_trait_pass_t = typename type_trait<T>::pass;

template <typename T>
using type_trait_take_t = typename type_trait<T>::take;

} /* namespace sigc */

#endif /* SIGC_TYPE_TRAIT_H */
