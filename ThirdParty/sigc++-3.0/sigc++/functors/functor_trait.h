/*
 * Copyright 2003 - 2016, The libsigc++ Development Team
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
 */

#ifndef SIGC_FUNCTORS_FUNCTOR_TRAIT_H
#define SIGC_FUNCTORS_FUNCTOR_TRAIT_H

#include <sigc++/functors/mem_fun.h>
#include <sigc++/functors/ptr_fun.h>
#include <sigc++/type_traits.h>
#include <type_traits>

namespace sigc
{

/** @defgroup sigcfunctors Functors
 * Functors are copyable types that define operator()().
 *
 * Types that define operator()() overloads with different return types are referred to
 * as multi-type functors. Multi-type functors are only partially supported in libsigc++.
 *
 * Closures are functors that store all information needed to invoke a callback from operator()().
 *
 * Adaptors are functors that alter the signature of a functor's operator()().
 *
 * libsigc++ defines numerous functors, closures and adaptors.
 * Since libsigc++ is a callback library, most functors are also closures.
 * The documentation doesn't distinguish between functors and closures.
 *
 * The basic functor types libsigc++ provides are created with ptr_fun() and mem_fun()
 * and can be converted into slots implicitly.
 * The set of adaptors that ships with libsigc++ is documented in the @ref adaptors module.
 *
 */

/** Trait that specifies the appropriate functor type of any callable type.
 * Template specializations for function pointers and for class methods are provided.
 *
 * @tparam T_functor Functor type.
 *
 * @ingroup sigcfunctors
 */
template <typename T_functor>
struct functor_trait
{
  using functor_type = T_functor;
};

#ifndef DOXYGEN_SHOULD_SKIP_THIS
// detect the functor version of non-functor types.

// functor ptr fun:

template <typename T_return, typename... T_arg>
struct functor_trait<T_return (*)(T_arg...)>
{
  using functor_type = pointer_functor<T_return(T_arg...)>;
};

// functor mem fun:

template <typename T_return, typename T_obj, typename... T_arg>
struct functor_trait<T_return (T_obj::*)(T_arg...)>
{
  using functor_type = mem_functor<T_return (T_obj::*)(T_arg...), T_arg...>;
};

template <typename T_return, typename T_obj, typename... T_arg>
struct functor_trait<T_return (T_obj::*)(T_arg...) const>
{
  using functor_type = mem_functor<T_return (T_obj::*)(T_arg...) const, T_arg...>;
};

#endif // DOXYGEN_SHOULD_SKIP_THIS

} /* namespace sigc */
#endif /* SIGC_FUNCTORS_FUNCTOR_TRAIT_H */
