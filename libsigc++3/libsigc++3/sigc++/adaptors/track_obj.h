/*
 * Copyright 2013 - 2016, The libsigc++ Development Team
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

#ifndef SIGC_ADAPTORS_TRACK_OBJ_H
#define SIGC_ADAPTORS_TRACK_OBJ_H

#include <sigc++/adaptors/adapts.h>
#include <sigc++/adaptors/tuple_visitor_visit_each.h>
#include <sigc++/limit_reference.h>
#include <sigc++/tuple-utils/tuple_for_each.h>

namespace sigc
{

/** @defgroup track_obj track_obj()
 * sigc::track_obj() tracks trackable objects, referenced from a functor.
 * It can be useful when you assign a C++11 lambda expression or a std::function<>
 * to a slot, or connect it to a signal, and the lambda expression or std::function<>
 * contains references to sigc::trackable derived objects.
 *
 * The functor returned by sigc::track_obj() is formally an adaptor, but it does
 * not alter the signature, return type or behaviour of the supplied functor.
 * Up to 7 objects can be tracked. operator()() can have up to 7 arguments.
 *
 * @par Example:
 * @code
 * struct bar : public sigc::trackable {};
 * sigc::signal<void()> some_signal;
 * void foo(bar&);
 * {
 *   bar some_bar;
 *   some_signal.connect([&some_bar](){ foo(some_bar); });
 *     // NOT disconnected automatically when some_bar goes out of scope
 *   some_signal.connect(sigc::track_obj([&some_bar](){ foo(some_bar); }, some_bar);
 *     // disconnected automatically when some_bar goes out of scope
 * }
 * @endcode
 *
 * @newin{2,4}
 *
 * @ingroup adaptors
 */

/** track_obj_functor wraps a functor and stores a reference to a trackable object.
 * Use the convenience function track_obj() to create an instance of track_obj_functor.
 *
 * @tparam T_functor The type of functor to wrap.
 * @tparam T_obj The types of the trackable objects.
 *
 * @newin{2,4}
 *
 * @ingroup track_obj
 */
template <typename T_functor, typename... T_obj>
class track_obj_functor : public adapts<T_functor>
{
public:
  /** Constructs a track_obj_functor object that wraps the passed functor and
   * stores a reference to the passed trackable objects.
   * @param func Functor.
   * @param obj Trackable objects.
   */
  explicit track_obj_functor(const T_functor& func, const T_obj&... obj)
  : adapts<T_functor>(func), obj_(obj...)
  {
  }

  /** Invokes the wrapped functor.
   * @return The return value of the functor invocation.
   */
  decltype(auto) operator()() { return this->functor_(); }

  /** Invokes the wrapped functor passing on the arguments.
   * @param arg Arguments to be passed on to the functor.
   * @return The return value of the functor invocation.
   */
  template <typename... T_arg>
  decltype(auto) operator()(T_arg&&... arg)
  {
    return this->functor_.template operator()<type_trait_pass_t<T_arg>...>(
      std::forward<T_arg>(arg)...);
  }

#ifndef DOXYGEN_SHOULD_SKIP_THIS
  // protected:
  // public, so that visit_each() can access it.
  std::tuple<limit_reference<const T_obj>...> obj_;
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

}; // end class track_obj_functor

#ifndef DOXYGEN_SHOULD_SKIP_THIS
// template specialization of visitor<>::do_visit_each<>(action, functor):
/** Performs a functor on each of the targets of a functor.
 * The function overload for sigc::track_obj_functor performs a functor
 * on the functor and on the trackable object instances stored in the
 * sigc::track_obj_functor object.
 *
 * @newin{2,4}
 *
 * @ingroup track_obj
 */
template <typename T_functor, typename... T_obj>
struct visitor<track_obj_functor<T_functor, T_obj...>>
{
  template <typename T_action>
  static void do_visit_each(
    const T_action& action, const track_obj_functor<T_functor, T_obj...>& target)
  {
    sigc::visit_each(action, target.functor_);

    // Call sigc::visit_each(action, element) on each element in the
    //target.obj_ tuple:
    sigc::internal::tuple_for_each<internal::TupleVisitorVisitEach>(target.obj_, action);
  }
};
#endif // DOXYGEN_SHOULD_SKIP_THIS

/** Creates an adaptor of type sigc::track_obj_functor which wraps a functor.
 * @param func Functor that shall be wrapped.
 * @param obj Trackable objects.
 * @return Adaptor that executes func() on invocation.
 *
 * @newin{2,4}
 *
 * @ingroup track_obj
 */
template <typename T_functor, typename... T_obj>
inline decltype(auto)
track_obj(const T_functor& func, const T_obj&... obj)
{
  return track_obj_functor<T_functor, T_obj...>(func, obj...);
}

} /* namespace sigc */

#endif /* SIGC_ADAPTORS_TRACK_OBJ_H */
