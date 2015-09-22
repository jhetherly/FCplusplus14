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

  return 0;
}
