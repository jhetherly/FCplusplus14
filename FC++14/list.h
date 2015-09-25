#ifndef FCPP_LIST_H
#define FCPP_LIST_H

#include <iterator>
#include <memory>
#include <utility>
#include <type_traits>
#include <functional>

#include "FC++14/functoid.h"

namespace fcpp
{

// "recursively" defined list
// C++ doesn't allow for truely recursive type definitions :(
//
// Structure: (S = suspension (manages actual data), M = suspension manager, L = list)
//  S     S     S     S     =
//  ^     ^     ^     ^     ^
//  |     |     |     |     |
//  M---->M---->M---->M---->M
//  ^\    ^\    ^\    ^\    ^
//  | \   | \   | \   | \   |
//  |  \  |  \  |  \  |  \  |
//  |   \ |   \ |   \ |   \ |
//  |    v|    v|    v|    v|
//  L     L     L     L     L
//
//  The links between the M's are to preserve them in memory even after an L
//  object no longer points to it. The links directed from the M's to L's are 
//  really just functions that generate to requisite L on demand. Users 
//  typically hold on to the L on the lower left.

template<class T, class A = std::allocator<T>>
struct List;


namespace _impl
{

template<class T>
struct ListSuspensionManager : 
  std::enable_shared_from_this<ListSuspensionManager<T>> {
  using list_generator_type = std::function<List<T>(const List<T>&)>;
  using thunk_type = std::function<T()>;

  ListSuspensionManager() = delete;
  ListSuspensionManager(const ListSuspensionManager<T>&) = default;
  ListSuspensionManager(ListSuspensionManager<T>&&) = default;

  // next-to-last element
  ListSuspensionManager(T&& val) : 
    _thunk(make_suspension_for_value(std::move(val))), 
    _tail_gen{[](const List<T>&) {return List<T>();}} {}
  ListSuspensionManager(const T &val) : 
    _thunk(make_suspension_for_value(val)),
    _tail_gen{[](const List<T>&) {return List<T>();}} {}
  ListSuspensionManager(thunk_type f) : 
    _thunk(f),
    _tail_gen{[](const List<T>&) {return List<T>();}} {}

  // normal element
  ListSuspensionManager(T&& val, std::shared_ptr<const ListSuspensionManager<T>> tail) : 
    _thunk(make_suspension_for_value(std::move(val))), 
    _tail_gen{[tail](const List<T>&) {return List<T>(tail);}},
    _tail(tail) {}
  ListSuspensionManager(const T &val, std::shared_ptr<const ListSuspensionManager<T>> tail) : 
    _thunk(make_suspension_for_value(val)),
    _tail_gen{[tail](const List<T>&) {return List<T>(tail);}},
    _tail(tail) {}
  ListSuspensionManager(thunk_type f, std::shared_ptr<const ListSuspensionManager<T>> tail) : 
    _thunk(f),
    _tail_gen{[tail](const List<T>&) {return List<T>(tail);}},
    _tail(tail) {}

  // generators
  ListSuspensionManager(T&& val, list_generator_type tail_gen) : 
    _thunk(make_suspension_for_value(std::move(val))), 
    _tail_gen{tail_gen} {}
  ListSuspensionManager(const T &val, list_generator_type tail_gen) : 
    _thunk(make_suspension_for_value(val)),
    _tail_gen{tail_gen} {}
  ListSuspensionManager(thunk_type f, list_generator_type tail_gen) : 
    _thunk(f),
    _tail_gen{tail_gen} {}

  bool operator== (const ListSuspensionManager<T> &other) const
  {
    bool are_equal = true;
    if (_thunk && other._thunk) are_equal &= (_thunk() == other._thunk());
    else if ((!_thunk && other._thunk) || (_thunk && !other._thunk)) are_equal &= false;
    are_equal &= (_tail == other._tail);
    if ((!_tail_gen && other._tail_gen) || (_tail_gen && !other._tail_gen)) are_equal &= false;
    return are_equal;
  }
  bool operator!= (const ListSuspensionManager<T> &other) const {return !(*this == other);}
  std::shared_ptr<const ListSuspensionManager<T>> get_handle() const {return this->shared_from_this();}
  T operator() () {return _thunk();}
  bool is_last_element () const {return !(_tail_gen || _tail);}

  // "private:" stuff
  void _set_tail (std::shared_ptr<const ListSuspensionManager<T>> tail) const {_tail = tail;}

  thunk_type                                              _thunk;
  list_generator_type                                     _tail_gen;
  mutable std::shared_ptr<const ListSuspensionManager<T>> _tail;
};

}



template<class T, class A>
struct List {
  using list_generator_type = std::function<List<T>(const List<T>&)>;
  using thunk_type = std::function<T()>;
  // STL compliance
  struct const_iterator;

  // copy
  List (const List<T> &l) = default;
  // copy assign
  List& operator=(const List<T> &l) = default;
  // move
  List (List<T>&& l) = default;
  // move assign
  List& operator=(List<T>&& l) = default;

  // empty list
  List() = default;
  // List(NIL) {}

  // single value lists
  List (T&& val) : 
    _head(std::make_shared<const _impl::ListSuspensionManager<T>>(std::move(val))) {}
  List (const T &val) : 
    _head(std::make_shared<const _impl::ListSuspensionManager<T>>(val)) {}
  List (thunk_type f) : 
    _head(std::make_shared<const _impl::ListSuspensionManager<T>>(f)) {}
  List (const std::shared_ptr<const _impl::ListSuspensionManager<T>> &m) : 
    _head(m) {}

  // concat lists
  List (T&& val, List<T>&& l) : 
    _head(std::make_shared<const _impl::ListSuspensionManager<T>>(std::move(val), l._head)) {}
  List (const T &val, List<T>&& l) : 
    _head(std::make_shared<const _impl::ListSuspensionManager<T>>(std::move(val), l._head)) {}
  List (T&& val, const List<T> &l) : 
    _head(std::make_shared<const _impl::ListSuspensionManager<T>>(val, l._head)) {}
  List (const T &val, const List<T> &l) : 
    _head(std::make_shared<const _impl::ListSuspensionManager<T>>(val, l._head)) {}

  // list generators
  List (T&& val, list_generator_type f) : 
    _head(std::make_shared<const _impl::ListSuspensionManager<T>>(std::move(val), f)) {}
  List (const T &val, list_generator_type f) : 
    _head(std::make_shared<const _impl::ListSuspensionManager<T>>(val, f)) {}

  bool operator== (const List &l) const {if (!l._head || !_head) return l._head == _head; return *l._head == *_head;}
  bool operator!= (const List &l) const {return !(l == *this);}

  const auto& operator() () const & {return *this;}
  auto operator() () && {return std::move(*this);}
  T head () const
  {
    if (_head) return _head->_thunk();
    throw("tried to evaluate an empty list");  // TODO: throw for now (possibly use Maybe monad in future)
  }
  List<T> tail () const
  {
    if (_head) {
      if (_head->is_last_element()) return _head->_tail_gen(*this);
      if (!_head->_tail && _head->_tail_gen) _head->_set_tail(_head->_tail_gen(*this)._head);
      return List<T>(_head->_tail);
    }
    throw("tried to evaluate an empty list");  // TODO: throw for now (possibly use Maybe monad in future)
  }
  bool is_empty () const {return static_cast<bool>(_head);}
  list_generator_type get_generator() const {return _head->_tail_gen;}

  const_iterator begin() const {return const_iterator{*this};}
  const_iterator cbegin() const {return const_iterator{*this};}
  const_iterator end() const {return const_iterator{List<T>()};}
  const_iterator cend() const {return const_iterator{List<T>()};}

  // "private:" stuff
  std::shared_ptr<const _impl::ListSuspensionManager<T>> _head;

  struct const_iterator {
    typedef typename A::difference_type difference_type;
    typedef typename A::value_type value_type;
    typedef typename A::reference const_reference;
    typedef typename A::pointer const_pointer;
    typedef std::input_iterator_tag iterator_category;

    const_iterator () = default;
    const_iterator (const const_iterator&) = default;
    ~const_iterator() {}

    const_iterator& operator=(const const_iterator&) = default;
    bool operator==(const const_iterator &rhs) const {return _element == rhs._element;}
    bool operator!=(const const_iterator &rhs) const {return _element != rhs._element;}

    const_iterator& operator++() {_element = _element.tail(); return *this;}
    const_iterator operator++(int) {const_iterator it{_element}; _element = _element.tail(); return it;}

    T operator*() const {return _element.head();}
    T operator->() const {return _element.head();}

    List<T> _element;
  };
};


}

#endif
