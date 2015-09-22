#include <iostream>
#include <random>
#include <chrono>

#include "FC++14/functoid.h"
#include "FC++14/list.h"


using namespace std::chrono;
using namespace fcpp;

int main ()
{
  List<int> l;
  // auto l2 = cons(1, cons(2, l));
  auto l2 = cons(0) < cons(1) < cons(2, l);
  std::cout << head(l2)() << std::endl;
  std::cout << head(tail(l2))() << std::endl;
  std::cout << head(tail(tail(l2)))() << std::endl;
  auto l3 = enumFrom(1,2);
  //std::cout << head(l3)() << std::endl;

  auto factorial = fix([](auto&& self, long double n) -> long double
      { return n < 2.0 ? 1.0 : n * self(self, n - 1.0); });

  std::cout << factorial(100000) << std::endl;

  // auto e = fix([](auto&& self, auto&& x1, auto&& x2)->List<typename std::decay<decltype(x1)>::type>
  //   {using list_type = List<typename std::decay<decltype(x1)>::type>;
  //   return list_type(std::forward<decltype(x1)>(x1), self(std::forward<decltype(self)>(self), std::forward<decltype(x2)>(x2), x2 + x2 - x1));});
  auto e = [](auto&& self, auto&& x1, auto&& x2)->List<typename std::decay<decltype(x1)>::type>
    {using list_type = List<typename std::decay<decltype(x1)>::type>;
    return list_type(std::forward<decltype(x1)>(x1), self(self, std::forward<decltype(x2)>(x2), x2 + x2 - x1));};

  e(e,1,2);

  return 0;
}
