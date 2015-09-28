#include <iostream>
#include <random>
#include <chrono>

#include "FC++14/prelude.h"


using namespace std::chrono;
using namespace fcpp;

int main ()
{
  // // random number generation setup
  // std::random_device rd;
  // std::mt19937 gen(rd());
  // std::uniform_int_distribution<> dis(-10, 10);
  // auto random_num = [&gen, &dis]() {return dis(gen);};
  // setup clock for timing
  auto start = steady_clock::now();
  auto end = steady_clock::now();
  auto ave_diff = duration <double, std::nano> (end - start).count();

  List<int> l1;
  std::cout << std::boolalpha << nil(l1)() << std::endl;
  List<int> test(9);
  std::cout << test.head() << std::endl;
  std::cout << std::boolalpha << nil(test)() << "\t\t" << (nil * tail(test))() << std::endl;
  List<int> test2(8, test);
  auto l2 = cons(0) * cons(1) * cons(2, l1);
  std::cout << head(l2)() << std::endl;
  std::cout << head(tail(l2))() << std::endl;
  std::cout << head(tail(tail(l2)))() << std::endl;
  auto l3 = enumFrom(1,2);
  std::cout << (head * tail * tail * tail * tail(l3))() << std::endl;
  auto element10 = head * tail * tail * tail * tail * tail * tail * tail * tail * tail;
  std::cout << element10(l3)() << std::endl;
  auto l4 = enumFromTo(1,2,10);
  std::cout << element10(l4)() << std::endl;
  // tail * element10(l4); // compile time error
  for (auto e : l4())
    std::cout << e << "  ";
  std::cout << std::endl; 
  auto l5 = enumFromTo('a','b','z');
  for (auto e : l5())
    std::cout << e << "  ";
  std::cout << std::endl; 


  double sum = 0.0;
  long long large_loop = 100000;
  start = steady_clock::now();
  for (auto e : enumFromTo(1,2,large_loop)())
    sum += e;
  end = steady_clock::now();
  ave_diff = duration <double, std::nano> (end - start).count() / static_cast<decltype(ave_diff)>(large_loop);
  std::cout << "Average per-element time for summing enumerated " << large_loop << " numbers: " << ave_diff << " ns" << std::endl;

  start = steady_clock::now();
  for (long long i = 1; i <= large_loop; ++i)
    sum += i;
  end = steady_clock::now();
  ave_diff = duration <double, std::nano> (end - start).count() / static_cast<decltype(ave_diff)>(large_loop);
  std::cout << "Average per-element time for summing " << large_loop << " numbers: " << ave_diff << " ns" << std::endl;

  std::cout << std::endl; 

  return 0;
}
