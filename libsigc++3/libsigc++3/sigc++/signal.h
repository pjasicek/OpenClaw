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

#ifndef SIGC_SIGNAL_H
#define SIGC_SIGNAL_H

#include <list>
#include <sigc++/connection.h>
#include <sigc++/signal_base.h>
#include <sigc++/type_traits.h>
#include <sigc++/trackable.h>
#include <sigc++/functors/slot.h>
#include <sigc++/functors/mem_fun.h>
#include <tuple>
#include <utility>

namespace sigc
{

namespace internal
{

/** Special iterator over sigc::internal::signal_impl's slot list that holds extra data.
 * This iterators is for use in accumulators. operator*() executes
 * the slot. The return value is buffered, so that in an expression
 * like @code a = (*i) * (*i); @endcode the slot is executed only once.
 */
template <typename T_emitter, typename T_result = typename T_emitter::result_type>
struct slot_iterator_buf
{
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using iterator_category = std::bidirectional_iterator_tag;

  // These are needed just to make this a proper C++ iterator,
  // that can be used with standard C++ algorithms.
  using value_type = T_result;
  using reference = T_result&;
  using pointer = T_result*;

  using emitter_type = T_emitter;
  using slot_type = typename T_emitter::slot_type;

  using iterator_type = signal_impl::const_iterator_type;

  slot_iterator_buf() : c_(nullptr), invoked_(false) {}

  slot_iterator_buf(const iterator_type& i, const emitter_type* c) : i_(i), c_(c), invoked_(false)
  {
  }

  decltype(auto) operator*() const
  {
    if (!i_->empty() && !i_->blocked() && !invoked_)
    {
      r_ = (*c_)(static_cast<const slot_type&>(*i_));
      invoked_ = true;
    }
    return r_;
  }

  slot_iterator_buf& operator++()
  {
    ++i_;
    invoked_ = false;
    return *this;
  }

  slot_iterator_buf operator++(int)
  {
    slot_iterator_buf tmp(*this);
    ++i_;
    invoked_ = false;
    return tmp;
  }

  slot_iterator_buf& operator--()
  {
    --i_;
    invoked_ = false;
    return *this;
  }

  slot_iterator_buf operator--(int)
  {
    slot_iterator_buf tmp(*this);
    --i_;
    invoked_ = false;
    return tmp;
  }

  bool operator==(const slot_iterator_buf& src) const
  {
    return (!c_ || (i_ == src.i_));
  } /* If '!c_' the iterators are empty.
     * Unfortunately, empty stl iterators are not equal.
     * We are forcing equality so that 'first==last'
     * in the accumulator's emit function yields true. */

  bool operator!=(const slot_iterator_buf& src) const { return (c_ && (i_ != src.i_)); }

private:
  iterator_type i_;
  const emitter_type* c_;
  mutable T_result r_;
  mutable bool invoked_;
};

/** Template specialization of slot_iterator_buf for void return signals.
 */
template <typename T_emitter>
struct slot_iterator_buf<T_emitter, void>
{
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using iterator_category = std::bidirectional_iterator_tag;

  using emitter_type = T_emitter;
  using slot_type = typename T_emitter::slot_type;

  using iterator_type = signal_impl::const_iterator_type;

  slot_iterator_buf() : c_(nullptr), invoked_(false) {}

  slot_iterator_buf(const iterator_type& i, const emitter_type* c) : i_(i), c_(c), invoked_(false)
  {
  }

  void operator*() const
  {
    if (!i_->empty() && !i_->blocked() && !invoked_)
    {
      (*c_)(static_cast<const slot_type&>(*i_));
      invoked_ = true;
    }
  }

  slot_iterator_buf& operator++()
  {
    ++i_;
    invoked_ = false;
    return *this;
  }

  slot_iterator_buf operator++(int)
  {
    slot_iterator_buf tmp(*this);
    ++i_;
    invoked_ = false;
    return tmp;
  }

  slot_iterator_buf& operator--()
  {
    --i_;
    invoked_ = false;
    return *this;
  }

  slot_iterator_buf operator--(int)
  {
    slot_iterator_buf tmp(*this);
    --i_;
    invoked_ = false;
    return tmp;
  }

  bool operator==(const slot_iterator_buf& src) const { return i_ == src.i_; }

  bool operator!=(const slot_iterator_buf& src) const { return i_ != src.i_; }

private:
  iterator_type i_;
  const emitter_type* c_;
  mutable bool invoked_;
};

/** Temporary slot list used during signal emission.
 *  Through evolution this class is slightly misnamed.  It is now
 *  an index into the slot_list passed into it.  It simply keeps track
 *  of where the end of this list was at construction, and pretends that's
 *  the end of your list.  This way you may connect during emission without
 *  inadvertently entering an infinite loop, as well as make other
 *  modifications to the slot_list at your own risk.
 */
struct temp_slot_list
{
  using slot_list = signal_impl::slot_list;
  using iterator = signal_impl::iterator_type;
  using const_iterator = signal_impl::const_iterator_type;

  explicit temp_slot_list(slot_list& slots) : slots_(slots)
  {
    placeholder = slots_.insert(slots_.end(), slot_base());
  }

  ~temp_slot_list() { slots_.erase(placeholder); }

  iterator begin() { return slots_.begin(); }
  iterator end() { return placeholder; }
  const_iterator begin() const { return slots_.begin(); }
  const_iterator end() const { return placeholder; }

private:
  slot_list& slots_;
  slot_list::iterator placeholder;
};

/** Abstracts signal emission.
 * This template implements the emit() function of signal_with_accumulator.
 * Template specializations are available to optimize signal
 * emission when no accumulator is used, for example when the template
 * argument @e T_accumulator is @p void.
 */
template <typename T_return, typename T_accumulator, typename... T_arg>
struct signal_emit
{
  using self_type = signal_emit<T_return, T_accumulator, T_arg...>;
  using slot_type = slot<T_return(T_arg...)>;

  /** Instantiates the class.
   * The parameters are stored in member variables. operator()() passes
   * the values on to some slot.
   */
  explicit signal_emit(type_trait_take_t<T_arg>... a) : a_(a...) {}

  /** Invokes a slot using the buffered parameter values.
   * @param slot Some slot to invoke.
   * @return The slot's return value.
   */
  T_return operator()(const slot_type& slot) const
  {
    const auto seq = std::make_index_sequence<std::tuple_size<decltype(a_)>::value>();
    return call_call_type_operator_parentheses_with_tuple(slot, a_, seq);
  }

  /** Executes a list of slots using an accumulator of type @e T_accumulator.
   * The arguments are buffered in a temporary instance of signal_emit.
   * @param a Arguments to be passed on to the slots.
   * @return The accumulated return values of the slot invocations as processed by the accumulator.
   */
  static decltype(auto) emit(const std::shared_ptr<internal::signal_impl>& impl, type_trait_take_t<T_arg>... a)
  {
    using slot_iterator_buf_type = internal::slot_iterator_buf<self_type, T_return>;

    T_accumulator accumulator;

    if (!impl)
      return accumulator(slot_iterator_buf_type(), slot_iterator_buf_type());

    signal_impl_holder exec(impl);
    const temp_slot_list slots(impl->slots_);

    self_type self(a...);
    return accumulator(
      slot_iterator_buf_type(slots.begin(), &self), slot_iterator_buf_type(slots.end(), &self));
  }

private:
  std::tuple<type_trait_take_t<T_arg>...> a_;

  // TODO_variadic: Replace this with std::experimental::apply() if that becomes standard
  // C++, or add our own implementation, to avoid code duplication.
  template <std::size_t... Is>
  decltype(auto) call_call_type_operator_parentheses_with_tuple(
    const slot_type& slot, const std::tuple<T_arg...>& tuple, std::index_sequence<Is...>) const
  {
    return (slot)(std::get<Is>(tuple)...);
  }
};

/** Abstracts signal emission.
 * This template specialization implements an optimized emit()
 * function for the case that no accumulator is used.
 */
template <typename T_return, typename... T_arg>
struct signal_emit<T_return, void, T_arg...>
{
private:
  using slot_type = slot<T_return(T_arg...)>;
  using call_type = typename slot_type::call_type;

public:

  /** Executes a list of slots.
   * The arguments are passed directly on to the slots.
   * The return value of the last slot invoked is returned.
   * @param first An iterator pointing to the first slot in the list.
   * @param last An iterator pointing to the last slot in the list.
   * @param a Arguments to be passed on to the slots.
   * @return The return value of the last slot invoked.
   */
  static decltype(auto) emit(const std::shared_ptr<internal::signal_impl>& impl, type_trait_take_t<T_arg>... a)
  {
    if (!impl || impl->slots_.empty())
      return T_return();

    signal_impl_holder exec(impl);
    T_return r_ = T_return();

    // Use this scope to make sure that "slots" is destroyed before "exec" is destroyed.
    // This avoids a leak on MSVC++ - see http://bugzilla.gnome.org/show_bug.cgi?id=306249
    {
      const temp_slot_list slots(impl->slots_);
      auto it = slots.begin();
      for (; it != slots.end(); ++it)
      {
        if (!it->empty() && !it->blocked())
          break;
      }

      if (it == slots.end())
      {
        // note that 'T_return r_();' doesn't work => define 'r_' after this line
        // and initialize as follows:
        return T_return();
      }

      r_ = (reinterpret_cast<call_type>(it->rep_->call_))(it->rep_, a...);
      for (++it; it != slots.end(); ++it)
      {
        if (it->empty() || it->blocked())
          continue;
        r_ = (reinterpret_cast<call_type>(it->rep_->call_))(it->rep_, a...);
      }
    }

    return r_;
  }
};

/** Abstracts signal emission.
 * This template specialization implements an optimized emit()
 * function for the case that no accumulator is used and the
 * return type is @p void.
 */
template <typename... T_arg>
struct signal_emit<void, void, T_arg...>
{
private:
  using slot_type = slot<void(T_arg...)>;
  using call_type = typename slot_type::call_type;

public:

  /** Executes a list of slots using an accumulator of type @e T_accumulator.
   * The arguments are passed directly on to the slots.
   * @param a Arguments to be passed on to the slots.
   */
  static decltype(auto) emit(const std::shared_ptr<internal::signal_impl>& impl, type_trait_take_t<T_arg>... a)
  {
    if (!impl || impl->slots_.empty())
      return;
    signal_impl_holder exec(impl);
    const temp_slot_list slots(impl->slots_);

    for (const auto& slot : slots)
    {
      if (slot.empty() || slot.blocked())
        continue;

      (reinterpret_cast<call_type>(slot.rep_->call_))(slot.rep_, a...);
    }
  }
};

} /* namespace internal */

/** Signal declaration.
 * signal_with_accumulator can be used to connect() slots that are invoked
 * during subsequent calls to emit(). Any functor or slot
 * can be passed into connect(). It is converted into a slot
 * implicitly.
 *
 * If you want to connect one signal to another, use make_slot()
 * to retrieve a functor that emits the signal when invoked.
 *
 * Be careful if you directly pass one signal into the connect()
 * method of another: a shallow copy of the signal is made and
 * the signal's slots are not disconnected until both the signal
 * and its clone are destroyed, which is probably not what you want!
 *
 * The following template arguments are used:
 * - @e T_return The desired return type for the emit() function (may be overridden by the
 * accumulator).
 * - @e T_arg Argument types used in the definition of emit().
 * - @e T_accumulator The accumulator type used for emission. The default
 * @p void means that no accumulator should be used, for example if signal
 * emission returns the return value of the last slot invoked.
 *
 * @ingroup signal
 */
template <typename T_return, typename T_accumulator, typename... T_arg>
class signal_with_accumulator : public signal_base
{
public:
  using slot_type = slot<T_return(T_arg...)>;

  /** Add a slot to the list of slots.
   * Any functor or slot may be passed into connect().
   * It will be converted into a slot implicitly.
   * The returned connection may be stored for disconnection
   * of the slot at some later point. It stays valid until
   * the slot is disconnected from the signal.
   * std::function<> and C++11 lambda expressions are functors.
   * These are examples of functors that can be connected to a signal.
   *
   * %std::bind() creates a functor, but this functor typically has an
   * %operator()() which is a variadic template.
   * Our functor_trait can't deduce the result type
   * of such a functor. If you first assign the return value of %std::bind()
   * to a std::function, you can connect the std::function to a signal.
   *
   * @param slot_ The slot to add to the list of slots.
   * @return A connection.
   */
  connection connect(const slot_type& slot_)
  {
    auto iter = signal_base::connect(slot_);
    auto& slot_base = *iter;
    return connection(slot_base);
  }

  /** Add a slot to the list of slots.
   * @see connect(const slot_type& slot_).
   *
   * @newin{2,8}
   */
  connection connect(slot_type&& slot_)
  {
    auto iter = signal_base::connect(std::move(slot_));
    auto& slot_base = *iter;
    return connection(slot_base);
  }

  /** Triggers the emission of the signal.
   * During signal emission all slots that have been connected
   * to the signal are invoked unless they are manually set into
   * a blocking state. The parameters are passed on to the slots.
   * If @e T_accumulated is not @p void, an accumulator of this type
   * is used to process the return values of the slot invocations.
   * Otherwise, the return value of the last slot invoked is returned.
   * @param a Arguments to be passed on to the slots.
   * @return The accumulated return values of the slot invocations.
   */
  decltype(auto) emit(type_trait_take_t<T_arg>... a) const
  {
    using emitter_type = internal::signal_emit<T_return, T_accumulator, T_arg...>;
    return emitter_type::emit(impl_, a...);
  }

  /** Triggers the emission of the signal (see emit()). */
  decltype(auto) operator()(type_trait_take_t<T_arg>... a) const { return emit(a...); }

  /** Creates a functor that calls emit() on this signal.
   * @code
   * sigc::mem_fun(mysignal, &sigc::signal_with_accumulator::emit)
   * @endcode
   * yields the same result.
   * @return A functor that calls emit() on this signal.
   */
  decltype(auto) make_slot() const
  {
    //TODO: Instead use std::result_of<> on the static emitter_type::emit()
    using result_type = typename internal::member_method_result<decltype(&signal_with_accumulator::emit)>::type;
    return bound_mem_functor<result_type (signal_with_accumulator::*)(type_trait_take_t<T_arg>...)
                               const,
      type_trait_take_t<T_arg>...>(*this, &signal_with_accumulator::emit);
  }

  signal_with_accumulator() = default;

  signal_with_accumulator(const signal_with_accumulator& src) : signal_base(src) {}

  signal_with_accumulator(signal_with_accumulator&& src) : signal_base(std::move(src)) {}

  signal_with_accumulator& operator=(const signal_with_accumulator& src)
  {
    signal_base::operator=(src);
    return *this;
  }

  signal_with_accumulator& operator=(signal_with_accumulator&& src)
  {
    signal_base::operator=(std::move(src));
    return *this;
  }
};

/** signal can be used to connect() slots that are invoked
 * during subsequent calls to emit(). Any functor or slot
 * can be passed into connect(). It is converted into a slot
 * implicitly.
 *
 * If you want to connect one signal to another, use make_slot()
 * to retrieve a functor that emits the signal when invoked.
 *
 * Be careful if you directly pass one signal into the connect()
 * method of another: a shallow copy of the signal is made and
 * the signal's slots are not disconnected until both the signal
 * and its clone are destroyed, which is probably not what you want!
 *
 * The template arguments determine the function signature of
 * the emit() function:
 * - @e T_return The desired return type of the emit() function. * - @e T_arg Argument types used in
 * the definition of emit().
 *
 * For instance, to declare a signal whose connected slot returns void and takes
 * two parameters of bool and int:
 * @code
 * sigc::signal<void(bool, int)> some_signal;
 * @endcode
 *
 * To specify an accumulator type the nested class signal::accumulated can be used.
 *
 * @par Example:
 * @code
 * void foo(int) {}
 * sigc::signal<void, long> sig;
 * sig.connect(sigc::ptr_fun(&foo));
 * sig.emit(19);
 * @endcode
 *
 * @ingroup signal
 */
template <typename T_return, typename... T_arg>
class signal;

template <typename T_return, typename... T_arg>
class signal<T_return(T_arg...)> : public signal_with_accumulator<T_return, void, T_arg...>
{
public:
  using accumulator_type = void;

  /** Like sigc::signal but the additional template parameter @e T_accumulator
   * defines the accumulator type that should be used.
   *
   * An accumulator is a functor that uses a pair of special iterators
   * to step through a list of slots and calculate a return value
   * from the results of the slot invocations. The iterators' operator*()
   * executes the slot. The return value is buffered, so that in an expression
   * like @code a = (*i) * (*i); @endcode the slot is executed only once.
   *
   * @par Example 1:
   * This accumulator calculates the arithmetic mean value:
   * @code
   * struct arithmetic_mean_accumulator
   * {
   *   template<typename T_iterator>
   *   double operator()(T_iterator first, T_iterator last) const
   *   {
   *     double value_ = 0;
   *     int n_ = 0;
   *     for (; first != last; ++first, ++n_)
   *       value_ += *first;
   *     return value_ / n_;
   *   }
   * };
   * @endcode
   *
   * @par Example 2:
   * This accumulator stops signal emission when a slot returns zero:
   * @code
   * struct interruptable_accumulator
   * {
   *   template<typename T_iterator>
   *   bool operator()(T_iterator first, T_iterator last) const
   *   {
   *     for (; first != last; ++first, ++n_)
   *       if (!*first) return false;
   *     return true;
   *   }
   * };
   * @endcode
   *
   * @ingroup signal
   */
  template <typename T_accumulator>
  class accumulated : public signal_with_accumulator<T_return, T_accumulator, T_arg...>
  {
  public:
    accumulated() = default;
    accumulated(const accumulated& src)
    : signal_with_accumulator<T_return, T_accumulator, T_arg...>(src)
    {
    }
  };

  signal() = default;

  signal(const signal& src) : signal_with_accumulator<T_return, accumulator_type, T_arg...>(src) {}

  signal(signal&& src)
  : signal_with_accumulator<T_return, accumulator_type, T_arg...>(std::move(src))
  {
  }

  signal& operator=(const signal& src)
  {
    signal_with_accumulator<T_return, accumulator_type, T_arg...>::operator=(src);
    return *this;
  }

  signal& operator=(signal&& src)
  {
    signal_with_accumulator<T_return, accumulator_type, T_arg...>::operator=(std::move(src));
    return *this;
  }
};

} /* namespace sigc */

#endif /* SIGC_SIGNAL_H */
