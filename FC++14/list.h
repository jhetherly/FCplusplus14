#ifndef FCPP_LIST_H
#define FCPP_LIST_H

#include <memory>
#include <utility>
#include <type_traits>
#include <functional>

#include "FC++14/functoid.h"

namespace fcpp
{

struct NIL_t {
  NIL_t () {}
  template<class T1, class T2>
  NIL_t (T1&& t1, T2&& t2) {}
  const auto& operator() () const {return *this;}
}NIL;


template<class T>
struct List {
  struct Item;

  List() {}
  template<class C>//,
  //   typename std::enable_if<!std::is_same<typename std::decay<C>::type, NIL_t>::value, int>::type = 0>
  List(T&& v, C&& tail) : _head(std::make_shared<const Item>(std::move(v), tail()._head)) {}
  // template<class C,
  //   typename std::enable_if<std::is_same<typename std::decay<C>::type, NIL_t>::value, int>::type = 0>
  // List(T&& v, C&& tail) : _head(std::make_shared<const Item>(std::move(v), nullptr)) {}
  template<class C>//,
  //  typename std::enable_if<!std::is_same<typename std::decay<C>::type, NIL_t>::value, int>::type = 0>
  List(const T &v, C&& tail) : _head(std::make_shared<const Item>(v, tail()._head)) {}
  // template<class C,
  //   typename std::enable_if<std::is_same<typename std::decay<C>::type, NIL_t>::value, int>::type = 0>
  // List(const T &v, C&& tail) : _head(std::make_shared<const Item>(v, nullptr)) {}
  template<class C>//,
  //  typename std::enable_if<!std::is_same<typename std::decay<C>::type, NIL_t>::value, int>::type = 0>
  List(std::function<T()> thunk, C&& tail) : _head(std::make_shared<const Item>(thunk, tail()._head)) {}
  // template<class C,
  //   typename std::enable_if<std::is_same<typename std::decay<C>::type, NIL_t>::value, int>::type = 0>
  // List(std::function<T()> thunk, C&& tail) : _head(std::make_shared<const Item>(thunk, nullptr)) {}
  List(const std::shared_ptr<const Item> &item) : _head(std::make_shared<const Item>(*item)) {}

  const auto& operator() () const
  {
    return *this;
  }

  std::shared_ptr<const Item> _head;

  struct Item
  {
    Item() = delete;
    Item(const Item &other) = default;
    Item(Item&& other) = default;
    Item(T&& v, std::shared_ptr<const Item> const & tail) : _susp(make_suspension_for_value(std::move(v))), _next(tail) {}
    Item(const T &v, std::shared_ptr<const Item> const & tail) : _susp(make_suspension_for_value(v)), _next(tail) {}
    Item(std::function<T()> thunk, std::shared_ptr<const Item> const & tail) : _susp(thunk), _next(tail) {}
    std::function<T()> _susp;
    std::shared_ptr<const Item> _next;
  };
};



// ///////////////////
// container functions
// ///////////////////
auto nil = make_curriable<1>([](auto&& c) 
    {return !static_cast<bool>(c()._head);});

auto cons = make_curriable<2>([](auto&& val, auto&& c) 
    {typename std::decay<decltype(c())>::type temp(std::forward<decltype(val)>(val), std::forward<decltype(c)>(c));
    return temp;});

auto head = make_curriable<1>([](auto&& c) 
    {if (!nil(std::forward<decltype(c)>(c))()) return c()._head->_susp();
    throw("empty container");}); // throw for now

auto tail = make_curriable<1>([](auto&& c) 
    {using cont_type = typename std::decay<decltype(c())>::type;
    if (!nil(std::forward<decltype(c)>(c))()) return cont_type{c()._head->_next};
    throw("empty container");}); // throw for now


}

#endif
