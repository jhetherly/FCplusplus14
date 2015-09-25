#include <iostream>
#include <random>
#include <chrono>

#include "FC++14/functoid.h"
#include "FC++14/list.h"


using namespace std::chrono;
using namespace fcpp;

int main ()
{
  // auto factorial = fix([](auto&& self, long double n) -> long double
  //     { return n < 2.0 ? 1.0 : n * self(self, n - 1.0); });

  // std::cout << factorial(1000) << std::endl;

  List<int> l1;
  std::cout << std::boolalpha << nil(l1)() << std::endl;
  List<int> test(9);
  std::cout << test.head() << std::endl;
  std::cout << std::boolalpha << nil(test)() << "\t\t" << (nil < tail(test))() << std::endl;
  List<int> test2(8, test);
  auto l2 = cons(0) < cons(1) < cons(2, l1);
  std::cout << head(l2)() << std::endl;
  std::cout << head(tail(l2))() << std::endl;
  std::cout << head(tail(tail(l2)))() << std::endl;
  auto l3 = enumFrom(1,2);
  std::cout << (head < tail < tail < tail < tail(l3))() << std::endl;
  auto element10 = head < tail < tail < tail < tail < tail < tail < tail < tail < tail;
  std::cout << element10(l3)() << std::endl;
  auto l4 = enumFromTo(1,2,10);
  std::cout << element10(l4)() << std::endl;
  // tail < element10(l4); // compile time error
  for (auto e : l4())
    std::cout << e << "  ";
  std::cout << std::endl; 
  auto l5 = enumFromTo('a','b','z');
  for (auto e : l5())
    std::cout << e << "  ";
  std::cout << std::endl; 

  return 0;
}
