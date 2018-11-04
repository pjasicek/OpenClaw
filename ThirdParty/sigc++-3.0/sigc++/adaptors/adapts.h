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

#ifndef SIGC_ADAPTORS_ADAPTS_H
#define SIGC_ADAPTORS_ADAPTS_H

#include <sigc++config.h> //To get SIGC_TEMPLATE_KEYWORD_OPERATOR_OVERLOAD
#include <sigc++/visit_each.h>
#include <sigc++/functors/functor_trait.h>
#include <sigc++/functors/ptr_fun.h>
#include <sigc++/functors/mem_fun.h>
#include <sigc++/adaptors/adaptor_trait.h>

namespace sigc
{

// Doxygen (at least version 1.8.4) removes blank lines in a code block.
// That's why there are empty comment lines in the following code block.
/** Base type for adaptors.
 * sigc::adapts wraps adaptors, functors, function pointers and class methods.
 * It contains a single member functor which is always a sigc::adaptor_base.
 * The adaptor_type alias defines the exact type that is used
 * to store the adaptor, functor, function pointer or class method passed
 * into the constructor. It differs from @a T_functor unless @a T_functor
 * inherits from sigc::adaptor_base.
 *
 * @par Example of a simple adaptor:
 * @code
 * namespace my_ns
 * {
 * template <typename T_functor>
 * struct my_adaptor : public sigc::adapts<T_functor>
 * {
 *   //
 *   decltype(auto)
 *   operator()() const;
 *   //
 *   template <typename T_arg1>
 *   decltype(auto)
 *   operator()(T_arg1 arg1) const;
 *   //
 *   template <typename T_arg1, typename T_arg2>
 *   decltype(auto)
 *   operator()(T_arg1 arg1, T_arg2 arg2) const;
 *   //
 *   // Constructs a my_adaptor object that wraps the passed functor.
 *   // Initializes adapts<T_functor>::functor_, which is invoked from operator()().
 *   explicit my_adaptor(const T_functor& functor)
 *     : sigc::adapts<T_functor>(functor) {}
 * };
 * } // end namespace my_ns
 * //
 * // Specialization of sigc::visitor for my_adaptor.
 * namespace sigc
 * {
 * template <typename T_functor>
 * struct visitor<my_ns::my_adaptor<T_functor> >
 * {
 *   template <typename T_action>
 *   static void do_visit_each(const T_action& action,
 *                             const my_ns::my_adaptor<T_functor>& target)
 *   {
 *     sigc::visit_each(action, target.functor_);
 *   }
 * };
 * } // end namespace sigc
 * @endcode
 *
 * If you implement your own adaptor, you must also provide your specialization
 * of sigc::visitor<>::do_visit_each<>() that will forward the call to the functor(s)
 * your adapter is wrapping. Otherwise, pointers stored within the functor won't be
 * invalidated when a sigc::trackable object is destroyed and you can end up
 * executing callbacks on destroyed objects.
 *
 * Your specialization of sigc::visitor<> must be in namespace sigc.
 *
 * @ingroup adaptors
 */
template <typename T_functor>
struct adapts : public adaptor_base
{
private:
  using adaptor_type = typename adaptor_trait<T_functor>::adaptor_type;

public:
  /** Constructs an adaptor that wraps the passed functor.
   * @param functor Functor to invoke from operator()().
   */
  explicit adapts(const T_functor& functor) : functor_(functor) {}

  /// Adaptor that is invoked from operator()().
  mutable adaptor_type functor_;
};

} /* namespace sigc */
#endif /* SIGC_ADAPTORS_ADAPTS_H */
