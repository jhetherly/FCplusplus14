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
struct List {
  struct Item;

  List() {}
  List(T&& v, List tail) : _head(std::make_shared<Item>(std::move(v), tail._head)) {}
  List(const T &v, List tail) : _head(std::make_shared<Item>(v, tail._head)) {}

  const auto& operator() () const
  {
    return *this;
  }

  std::shared_ptr<const Item> _head;

  struct Item
  {
    Item(T&& v, std::shared_ptr<Item> const & tail) : _susp(make_suspension_for_value(std::move(v))), _next(tail) {}
    Item(const T &v, std::shared_ptr<Item> const & tail) : _susp(make_suspension_for_value(v)), _next(tail) {}
    std::function<T()> _susp;
    std::shared_ptr<const Item> _next;
  };
};


// //////////////
// list functions
// //////////////
// // template<class T>
// // auto nil_impl (const List<T> &l)
// template<class I>
// auto nil_impl (I&& i)
// {
//   return static_cast<bool>(i._head);
// }
// auto nil = make_curriable(nil_impl);
auto nil = make_curriable<1>([](auto&& c) 
    {return static_cast<bool>(c._head);});

// template<class T>
// auto cons_impl (const T& v, const List<T> &l)
// {
//   return List<T>(c, l);
// }
// auto cons = make_curriable(cons_impl);
auto cons = make_curriable<2>([](auto&& val, auto&& c) 
    {typename std::decay<decltype(c)>::type temp(std::forward<decltype(val)>(val), std::forward<decltype(c)>(c));
    return temp;});

// template<class T>
// auto head_impl (const List<T> &l)
// {
//   if (l._head)
//     return l._head._susp();
//   throw("empty list"); // throw for now
// }
// auto head = make_curriable(head_impl);
auto head = make_curriable<1>([](auto&& c) 
    {if (l._head) return l._head._susp();
    throw("empty container");}); // throw for now

// template<class T>
// auto tail_impl (const List<T> &l)
// {
//   if (l._head)
//     return l._head._next;
//   throw("empty list"); // throw for now
// }
// auto tail = make_curriable(tail_impl);
auto tail = make_curriable<1>([](auto&& c) 
    {if (l._head) return l._head._next;
    throw("empty container");}); // throw for now


}

#endif
