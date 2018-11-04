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

#ifndef SIGC_ADAPTORS_ADAPTOR_TRAIT_H
#define SIGC_ADAPTORS_ADAPTOR_TRAIT_H

#include <sigc++config.h> //To get SIGC_TEMPLATE_KEYWORD_OPERATOR_OVERLOAD
#include <sigc++/visit_each.h>
#include <sigc++/functors/functor_trait.h>
#include <sigc++/functors/ptr_fun.h>
#include <sigc++/functors/mem_fun.h>
#include <sigc++/adaptors/adaptor_base.h>

/*
 * The idea here is simple.  To prevent the need to
 * specializing every adaptor for every type of functor
 * and worse non-functors like function pointers, we
 * will make an adaptor trait which can take ordinary
 * functors and make them adaptor functors for which
 * we will of course be able to avoid excess copies.
 * (in theory)
 *
 * this all depends on partial specialization to allow
 * us to do
 *   functor_.template operator() <types> (args);
 *
 *
 * I don't understand much of the above. However, I can
 * see that adaptors are implemented like they are because
 * there is no way to extract the return type and the argument
 * types from a functor type. Therefore, operator() is templated.
 * It's instantiated in slot_call#<>::operator() where the
 * argument types are known. The return type is finally determined
 * via the callof<> template - a tricky way to detect the return
 * type of a functor when the argument types are known. Martin.
 */

namespace sigc
{

/** @defgroup adaptors Adaptors
 * Adaptors are functors that alter the signature of a functor's
 * operator()().
 *
 * The adaptor types libsigc++ provides
 * are created with bind(), bind_return(), hide(), hide_return(),
 * retype_return(), retype(), compose(), exception_catch(), track_obj()
 * and group().
 *
 * You can easily derive your own adaptor type from sigc::adapts.
 */

/** Converts an arbitrary functor into an adaptor type.
 * All adaptor types in libsigc++ have
 * a <tt>template operator()</tt> member of every argument count
 * they support. These functions in turn invoke a stored adaptor's
 * <tt>template operator()</tt>, processing the arguments and return
 * value in a characteristic manner. Explicit function template
 * instantiation is used to pass type hints thus saving copy costs.
 *
 * adaptor_functor is a glue between adaptors and arbitrary functors
 * that just passes on the arguments. You won't use this type directly.
 *
 * The template argument @e T_functor determines the type of stored
 * functor.
 *
 * @ingroup adaptors
 */
template <typename T_functor>
struct adaptor_functor : public adaptor_base
{
  /** Invokes the wrapped functor passing on the arguments.
   * @return The return value of the functor invocation.
   */
  decltype(auto) operator()() const { return functor_(); }

  /** Invokes the wrapped functor passing on the arguments.
   * @param arg Arguments to be passed on to the functor.
   * @return The return value of the functor invocation.
   */
  template <typename... T_arg>
  decltype(auto) operator()(T_arg&&... arg) const
  {
    return functor_(std::forward<T_arg>(arg)...);
  }

  /// Constructs an invalid functor.
  adaptor_functor() = default;

  /** Constructs an adaptor_functor object that wraps the passed functor.
   * @param functor Functor to invoke from operator()().
   */
  explicit adaptor_functor(const T_functor& functor) : functor_(functor) {}

  /** Constructs an adaptor_functor object that wraps the passed (member)
   * function pointer.
   * @param type Pointer to function or class method to invoke from operator()().
   */
  template <typename T_type>
  explicit adaptor_functor(const T_type& type) : functor_(type)
  {
  }

  /// Functor that is invoked from operator()().
  mutable T_functor functor_;
};

#ifndef DOXYGEN_SHOULD_SKIP_THIS
// template specialization of visitor<>::do_visit_each<>(action, functor):
/** Performs a functor on each of the targets of a functor.
 * The function overload for sigc::adaptor_functor performs a functor
 * on the functor stored in the sigc::adaptor_functor object.
 *
 * @ingroup adaptors
 */
template <typename T_functor>
struct visitor<adaptor_functor<T_functor>>
{
  template <typename T_action>
  static void do_visit_each(const T_action& action, const adaptor_functor<T_functor>& target)
  {
    sigc::visit_each(action, target.functor_);
  }
};
#endif // DOXYGEN_SHOULD_SKIP_THIS

/** Trait that specifies what is the adaptor version of a functor type.
 * Template specializations exist for sigc::adaptor_base-derived functors,
 * for function pointers and for class methods.
 *
 * The template argument @e T_functor is the functor type to convert.
 * @e I_isadaptor indicates whether @e T_functor inherits from sigc::adaptor_base.
 *
 * @ingroup adaptors
 */
template <typename T_functor, bool I_isadaptor = std::is_base_of<adaptor_base, T_functor>::value>
struct adaptor_trait;


/** Trait that specifies the adaptor version of a functor type.
 * This template specialization is used for types that inherit from adaptor_base.
 * adaptor_type is equal to @p T_functor in this case.
 */
template <typename T_functor>
struct adaptor_trait<T_functor, true>
{
  using adaptor_type = T_functor;
};

/** Trait that specifies the adaptor version of a functor type.
 * This template specialization is used for arbitrary functors,
 * for function pointers and for class methods.
 * The latter are converted into @p pointer_functor or @p mem_functor types.
 * adaptor_type is equal to @p adaptor_functor<functor_type>.
 */
template <typename T_functor>
struct adaptor_trait<T_functor, false>
{
private:
  using functor_type = typename functor_trait<T_functor>::functor_type;

public:
  using adaptor_type = adaptor_functor<functor_type>;
};

} /* namespace sigc */
#endif /* SIGC_ADAPTORS_ADAPTOR_TRAIT_H */
