/*
 * Copyright 2015 - 2016, The libsigc++ Development Team
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

#ifndef SIGC_TUPLE_VISITOR_VISIT_EACH_H
#define SIGC_TUPLE_VISITOR_VISIT_EACH_H

#include <tuple>

namespace sigc
{

namespace internal
{

#ifndef DOXYGEN_SHOULD_SKIP_THIS
template <typename T_element>
struct TupleVisitorVisitEach
{
  template <typename T_action>
  constexpr static void visit(const T_element& element, T_action&& action)
  {
    sigc::visit_each(std::forward<T_action>(action), element);
  }
};
#endif // DOXYGEN_SHOULD_SKIP_THIS

} // namespace internal

} // namespace sigc

#endif /* SIGC_TUPLE_VISITOR_VISIT_EACH_H */
