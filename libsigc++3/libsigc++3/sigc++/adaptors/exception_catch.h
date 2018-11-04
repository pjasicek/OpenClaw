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

#ifndef SIGC_ADAPTORS_EXCEPTION_CATCH_H
#define SIGC_ADAPTORS_EXCEPTION_CATCH_H
#include <sigc++/adaptors/adapts.h>

namespace sigc
{

/*
   functor adaptor:  exception_catch(functor, catcher)

   usage:


   Future directions:
     The catcher should be told what type of return it needs to
   return for multiple type functors,  to do this the user
   will need to derive from catcher_base.
*/
/** @defgroup exception_catch exception_catch()
 * sigc::exception_catch() catches an exception thrown from within
 * the wrapped functor and directs it to a catcher functor.
 * This catcher can then rethrow the exception and catch it with the proper type.
 *
 * Note that the catcher is expected to return the same type
 * as the wrapped functor so that normal flow can continue.
 *
 * Catchers can be cascaded to catch multiple types, because uncaught
 * rethrown exceptions proceed to the next catcher adaptor.
 *
 * @par Examples:
 * @code
 * struct my_catch
 * {
 *   int operator()()
 *   {
 *     try { throw; }
 *     catch (std::range_error e) // catch what types we know
 *       { std::cerr << "caught " << e.what() << std::endl; }
 *     return 1;
 *   }
 * }
 * int foo(); // throws std::range_error
 * sigc::exception_catch(&foo, my_catch())();
 * @endcode
 *
 * The functor sigc::exception_catch() returns can be directly passed into
 * sigc::signal::connect().
 *
 * @par Example:
 * @code
 * sigc::signal<int()> some_signal;
 * some_signal.connect(sigc::exception_catch(&foo, my_catch));
 * @endcode
 *
 * @ingroup adaptors
 */

template <typename T_functor, typename T_catcher>
struct exception_catch_functor : public adapts<T_functor>
{
  decltype(auto) operator()()
  {
    try
    {
      return this->functor_();
    }
    catch (...)
    {
      return catcher_();
    }
  }

  template <typename... T_arg>
  decltype(auto) operator()(T_arg... a)
  {
    try
    {
      return this->functor_.template operator()<type_trait_pass_t<T_arg>...>(a...);
    }
    catch (...)
    {
      return catcher_();
    }
  }

  exception_catch_functor(const T_functor& func, const T_catcher& catcher)
  : adapts<T_functor>(func), catcher_(catcher)
  {
  }

  T_catcher catcher_;
};

#ifndef DOXYGEN_SHOULD_SKIP_THIS
// template specialization of visitor<>::do_visit_each<>(action, functor):
template <typename T_functor, typename T_catcher>
struct visitor<exception_catch_functor<T_functor, T_catcher>>
{
  template <typename T_action>
  static void do_visit_each(const T_action& action,
    const exception_catch_functor<T_functor, T_catcher>& target)
  {
    sigc::visit_each(action, target.functor_);
    sigc::visit_each(action, target.catcher_);
  }
};
#endif // DOXYGEN_SHOULD_SKIP_THIS

template <typename T_functor, typename T_catcher>
inline decltype(auto)
exception_catch(const T_functor& func, const T_catcher& catcher)
{
  return exception_catch_functor<T_functor, T_catcher>(func, catcher);
}

} /* namespace sigc */
#endif /* SIGC_ADAPTORS_EXCEPTION_CATCH_H */
