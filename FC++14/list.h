#ifndef FCPP_LIST_H
#define FCPP_LIST_H

#include <memory>
#include <utility>
#include <type_traits>
#include <functional>

#include "FC++14/functoid.h"

namespace fcpp
{

template<class T>
struct NewList;

template<class T>
struct ListSuspensionManager : 
  std::enable_shared_from_this<ListSuspensionManager<T>> {
  using list_generator_type = std::function<NewList<T>(const NewList<T>&)>;
  using thunk_type = std::function<T()>;

  ListSuspensionManager() = delete;
  ListSuspensionManager(const ListSuspensionManager<T>&) = default;
  ListSuspensionManager(ListSuspensionManager<T>&&) = default;

  // next-to-last element
  ListSuspensionManager(T&& val) : 
    _thunk(make_suspension_for_value(std::move(val))), 
    _tail_gen{[](const NewList<T>&) {return NewList<T>();}} {}
  ListSuspensionManager(const T &val) : 
    _thunk(make_suspension_for_value(val)),
    _tail_gen{[](const NewList<T>&) {return NewList<T>();}} {}
  ListSuspensionManager(thunk_type f) : 
    _thunk(f),
    _tail_gen{[](const NewList<T>&) {return NewList<T>();}} {}
  // normal element
  ListSuspensionManager(T&& val, std::shared_ptr<const ListSuspensionManager<T>> tail) : 
    _thunk(make_suspension_for_value(std::move(val))), 
    _tail_gen{[tail](const NewList<T>&) {return NewList<T>(tail);}},
    _tail(tail) {}
  ListSuspensionManager(const T &val, std::shared_ptr<const ListSuspensionManager<T>> tail) : 
    _thunk(make_suspension_for_value(val)),
    _tail_gen{[tail](const NewList<T>&) {return NewList<T>(tail);}},
    _tail(tail) {}
  ListSuspensionManager(thunk_type f, std::shared_ptr<const ListSuspensionManager<T>> tail) : 
    _thunk(f),
    _tail_gen{[tail](const NewList<T>&) {return NewList<T>(tail);}},
    _tail(tail) {}
  ListSuspensionManager(T&& val, list_generator_type tail_gen) : 
    _thunk(make_suspension_for_value(std::move(val))), 
    _tail_gen{tail_gen} {}
  ListSuspensionManager(const T &val, list_generator_type tail_gen) : 
    _thunk(make_suspension_for_value(val)),
    _tail_gen{tail_gen} {}
  ListSuspensionManager(thunk_type f, list_generator_type tail_gen) : 
    _thunk(f),
    _tail_gen{tail_gen} {}

  std::shared_ptr<const ListSuspensionManager<T>> get_handle() const {return this->shared_from_this();}
  T operator() () {return _thunk();}
  bool is_last_element () const {return !(_tail_gen || _tail);}
  void _set_tail (std::shared_ptr<const ListSuspensionManager<T>> tail) const {_tail = tail;}

  thunk_type                                              _thunk;
  list_generator_type                                     _tail_gen;
  mutable std::shared_ptr<const ListSuspensionManager<T>> _tail;
};



template<class T>
struct NewList {
  using list_generator_type = std::function<NewList<T>(const NewList<T>&)>;
  using thunk_type = std::function<T()>;

  // copy
  NewList (const NewList<T> &l) = default;

  // move
  NewList (NewList<T>&& l) = default;

  // empty list
  NewList() = default;
  // NewList(NIL) {}

  // single value lists
  NewList (T&& val) : 
    _head(std::make_shared<const ListSuspensionManager<T>>(std::move(val))) {}
  NewList (const T &val) : 
    _head(std::make_shared<const ListSuspensionManager<T>>(val)) {}
  NewList (thunk_type f) : 
    _head(std::make_shared<const ListSuspensionManager<T>>(f)) {}
  NewList (const std::shared_ptr<const ListSuspensionManager<T>> &m) : 
    _head(m) {}

  // concat lists
  NewList (T&& val, NewList<T>&& l) : 
    _head(std::make_shared<const ListSuspensionManager<T>>(std::move(val), l._head)) {}
  NewList (const T &val, NewList<T>&& l) : 
    _head(std::make_shared<const ListSuspensionManager<T>>(std::move(val), l._head)) {}
  NewList (T&& val, const NewList<T> &l) : 
    _head(std::make_shared<const ListSuspensionManager<T>>(val, l._head)) {}
  NewList (const T &val, const NewList<T> &l) : 
    _head(std::make_shared<const ListSuspensionManager<T>>(val, l._head)) {}

  // list generators
  NewList (T&& val, list_generator_type f) : 
    _head(std::make_shared<const ListSuspensionManager<T>>(std::move(val), f)) {}
  NewList (const T &val, list_generator_type f) : 
    _head(std::make_shared<const ListSuspensionManager<T>>(val, f)) {}

  const auto& operator() () const & {return *this;}
  auto operator() () && {return std::move(*this);}
  T head () const
  {
    if (_head) return _head->_thunk();
    throw("tried to evaluate an empty list");  // TODO: throw for now (possibly use Maybe monad in future)
  }
  NewList<T> tail () const
  {
    if (_head) {
      if (_head->is_last_element()) return _head->_tail_gen(*this);
      if (!_head->_tail && _head->_tail_gen) _head->_set_tail(_head->_tail_gen(*this)._head);
      return NewList<T>(_head->_tail);
    }
    throw("tried to evaluate an empty list");  // TODO: throw for now (possibly use Maybe monad in future)
  }
  bool is_empty () const {return static_cast<bool>(_head);}

  std::shared_ptr<const ListSuspensionManager<T>> _head;
};



// ///////////////////
// container functions
// ///////////////////
auto nil = make_curriable<1>([](auto&& c) 
    {return !static_cast<bool>(c().is_empty());});

auto cons = make_curriable<2>([](auto&& val, auto&& c) 
    {typename std::decay<decltype(c())>::type temp(std::forward<decltype(val)>(val), std::forward<decltype(c)>(c));
    return temp;});

auto head = make_curriable<1>([](auto&& c) 
    {if (!nil(std::forward<decltype(c)>(c))()) return c().head();
    throw("empty container");}); // throw for now

auto tail = make_curriable<1>([](auto&& c) 
    {if (!nil(std::forward<decltype(c)>(c))()) return c().tail();
    throw("empty container");}); // throw for now

auto enumFrom = make_curriable<2>([](auto&& x1, auto&& x2)->NewList<typename std::decay<decltype(x1)>::type>
    {using list_t = NewList<typename std::decay<decltype(x1)>::type>;
    // lambda in lambda technique
    auto func = [diff = x2 - x1](const list_t& l) 
    {return list_t(l.head()+diff, l._head->_tail_gen);};
    return list_t(std::forward<decltype(x1)>(x1), func);});

}

#endif
