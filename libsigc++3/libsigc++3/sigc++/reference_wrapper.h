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
#ifndef SIGC_REFERENCE_WRAPPER_H
#define SIGC_REFERENCE_WRAPPER_H

namespace sigc
{

template <typename T_type>
struct unwrap_reference
{
  using type = T_type;
};

template <typename T_type>
struct unwrap_reference<std::reference_wrapper<T_type>>
{
  using type = T_type&;
};

template <typename T_type>
struct unwrap_reference<std::reference_wrapper<const T_type>>
{
  using type = const T_type&;
};

template <typename T_type>
T_type&
unwrap(const std::reference_wrapper<T_type>& v)
{
  return v;
}

template <typename T_type>
const T_type&
unwrap(const std::reference_wrapper<const T_type>& v)
{
  return v;
}

} /* namespace sigc */

#endif /* SIGC_REFERENCE_WRAPPER_H */
