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

#ifndef SIGC_ADAPTORS_DEDUCE_RESULT_TYPE_H
#define SIGC_ADAPTORS_DEDUCE_RESULT_TYPE_H

namespace sigc
{

// TODO: Is this necessary now that we don't need deduce_result_type?
/** A hint to the compiler.
 * Functors which have all methods based on templates
 * should publicly inherit from this hint.
 *
 * Adaptors don't inherit from this type directly. They use
 * use sigc::adapts as a base type instead. sigc::adaptors
 * wraps arbitrary functor types as well as function pointers
 * and class methods.
 *
 * @ingroup adaptors
 */
struct adaptor_base
{
};

} /* namespace sigc */
#endif /* SIGC_ADAPTORS_DEDUCE_RESULT_TYPE_H */
