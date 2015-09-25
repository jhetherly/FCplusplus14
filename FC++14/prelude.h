#ifndef FCPP_PRELUDE_H
#define FCPP_PRELUDE_H

#include <utility>
#include <type_traits>
#include <functional>

#include "FC++14/functoid.h"
#include "FC++14/list.h"

namespace fcpp
{

// ///////////////////
// container functions
// ///////////////////

// works with any functor container with method "is_empty"
auto nil = make_curriable<1>([](auto&& c) 
    {return !c().is_empty();});

// works with any functor container c of type c_t with constructor of the form c_t(val, c)
auto cons = make_curriable<2>([](auto&& val, auto&& c) 
    {typename std::decay<decltype(c())>::type temp(std::forward<decltype(val)>(val), std::forward<decltype(c)>(c));
    return temp;});

// works with any functor container with method "head"
auto head = make_curriable<1>([](auto&& c) 
    {if (!nil(std::forward<decltype(c)>(c))()) return c().head();
    throw("empty container");}); // TODO: throw for now

// works with any functor container with method "tail"
auto tail = make_curriable<1>([](auto&& c) 
    {if (!nil(std::forward<decltype(c)>(c))()) return c().tail();
    throw("empty container");}); // TODO: throw for now

// //////////////////
// List<T> generators
// //////////////////

// uses List<T>'s generator constructors to make a list from the first two elements in an arithmetic series
auto enumFrom = make_curriable<2>([](auto&& x1, auto&& x2)
    {using list_t = List<typename std::decay<decltype(x1)>::type>;
    // lambda in lambda technique
    auto func = [diff = x2 - x1](const list_t& l) 
    {return list_t(l.head()+diff, l.get_generator());};
    return list_t(std::forward<decltype(x1)>(x1), func);});

// uses List<T>'s generator constructors to make a list from the first two elements in an arithmetic series up to the last element
auto enumFromTo = make_curriable<3>([](auto&& x1, auto&& x2, auto&& xn)
    {using list_t = List<typename std::decay<decltype(x1)>::type>;
    // lambda in lambda technique
    auto func = [diff = x2 - x1, xn](const list_t& l) 
    {auto next_val = l.head()+diff;
    if (next_val <= xn) return list_t(l.head()+diff, l.get_generator());
    return list_t();};
    return list_t(std::forward<decltype(x1)>(x1), func);});

}

#endif
