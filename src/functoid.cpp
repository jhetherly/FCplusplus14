#include <iostream>
#include <random>
#include <chrono>
#include <functional>
#include <list>

#include "FC++14/functoid.h"

struct FuncObject {
  auto operator() (double a, double b) const
  {
    return a + b;
  }
} func_object;


auto addtogether (double a, double b)
{return a + b;}




using namespace std::chrono;
using namespace fcpp;

int main ()
{
  std::cout << std::endl << "Timing comparisons for curried and composable functions" << std::endl;
  std::cout << "CAUTION: assignments seem to interact with timing - take all number with a grain of salt\n" << std::endl;

  // random number generation setup
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(-10, 10);
  auto random_num = [&gen, &dis]() {return dis(gen);};
  // setup clock for timing
  auto clock = steady_clock::now;
  auto start = clock();
  auto end = clock();
  auto ave_diff = duration <double, std::nano> (end - start).count();
  // max testing loop
  auto max_loop = 1000;
  // loop sum (so compiler doesn't optimize the loops away)
  long long loop_sum = 0;
  // generate random numbers
  std::list<int> random_nums1,
    random_nums2,
    random_nums3,
    random_nums4;
  for (auto i = 0; i < max_loop; ++i) {
    random_nums1.push_back(random_num());
    random_nums2.push_back(random_num());
    random_nums3.push_back(random_num());
    random_nums4.push_back(random_num());
  }



  // wrapping normal function
  std::cout << "Normal function" << std::endl;
  auto addtoo = make_curriable(addtogether);
  std::cout << "Value check: " << addtogether(2,3) << " == " << addtoo(2,3)() << std::endl;

  std::cout << "Priming loops ..." << std::endl;
  for (auto num1 : random_nums1)
    for (auto num2 : random_nums2)
      loop_sum += num1 + num2;

  // no function call
  start = clock();
  for (auto num1 : random_nums1)
    for (auto num2 : random_nums2)
      loop_sum += num1 + num2;
  end = clock();
  ave_diff = duration <double, std::nano> (end - start).count() / static_cast<decltype(ave_diff)>(random_nums1.size()*random_nums2.size());
  std::cout << "Average time for " << random_nums1.size()*random_nums2.size() << " addition of two numbers: " << ave_diff << " ns" << std::endl;

  // normal function call
  start = clock();
  for (auto num1 : random_nums1)
    for (auto num2 : random_nums2)
      loop_sum += addtogether(num1, num2);
  end = clock();
  ave_diff = duration <double, std::nano> (end - start).count() / static_cast<decltype(ave_diff)>(random_nums1.size()*random_nums2.size());
  std::cout << "Average time for " << random_nums1.size()*random_nums2.size() << " calls to (normal function) addtogether: " << ave_diff << " ns" << std::endl;

  // curriable-wrapped function call
  start = clock();
  for (auto num1 : random_nums1)
    for (auto num2 : random_nums2)
      loop_sum += addtoo(num1, num2)();
  end = clock();
  ave_diff = duration <double, std::nano> (end - start).count() / static_cast<decltype(ave_diff)>(random_nums1.size()*random_nums2.size());
  std::cout << "Average time for " << random_nums1.size()*random_nums2.size() << " calls to (curried function) addtoo: " << ave_diff << " ns" << std::endl;




  // wrapping function object
  std::cout << std::endl << "Function object" << std::endl;
  auto addobject = make_curriable(func_object, &FuncObject::operator());
  std::cout << "Value check: " << func_object(2,3) << " == " << addobject(2)(3)() << std::endl;

  std::cout << "Priming loops ..." << std::endl;
  for (auto num1 : random_nums1)
    for (auto num2 : random_nums2)
      loop_sum += num1 + num2;

  // no function call
  start = clock();
  for (auto num1 : random_nums1)
    for (auto num2 : random_nums2)
      loop_sum += num1 + num2;
  end = clock();
  ave_diff = duration <double, std::nano> (end - start).count() / static_cast<decltype(ave_diff)>(random_nums1.size()*random_nums2.size());
  std::cout << "Average time for " << random_nums1.size()*random_nums2.size() << " addition of two numbers: " << ave_diff << " ns" << std::endl;

  // normal function object call
  start = clock();
  for (auto num1 : random_nums1)
    for (auto num2 : random_nums2)
      loop_sum += func_object(num1, num2);
  end = clock();
  ave_diff = duration <double, std::nano> (end - start).count() / static_cast<decltype(ave_diff)>(random_nums1.size()*random_nums2.size());
  std::cout << "Average time for " << random_nums1.size()*random_nums2.size() << " calls to (normal function object) func_object: " << ave_diff << " ns" << std::endl;

  // curriable-wrapped function call
  start = clock();
  for (auto num1 : random_nums1)
    for (auto num2 : random_nums2)
      loop_sum += addobject(num1, num2)();
  end = clock();
  ave_diff = duration <double, std::nano> (end - start).count() / static_cast<decltype(ave_diff)>(random_nums1.size()*random_nums2.size());
  std::cout << "Average time for " << random_nums1.size()*random_nums2.size() << " calls to (curried function) addobject: " << ave_diff << " ns" << std::endl;




  // wrapping generic lambda
  std::cout << std::endl << "Generic lambda (2 arguments)" << std::endl;
  auto adder = make_curriable<2>([](auto x1, auto x2) {return x1 + x2 + x2;});
  auto _adder = [](auto x1, auto x2) {return x1 + x2 + x2;};
  std::cout << "Value check: " << _adder(2,3) << " == " << adder(2)(3)() << std::endl;

  std::cout << "Priming loops ..." << std::endl;
  for (auto num1 : random_nums1)
    for (auto num2 : random_nums2)
      loop_sum += num1 + num2 + num2;

  // no function call
  start = clock();
  for (auto num1 : random_nums1)
    for (auto num2 : random_nums2)
      loop_sum += num1 + num2 + num2;
  end = clock();
  ave_diff = duration <double, std::nano> (end - start).count() / static_cast<decltype(ave_diff)>(random_nums1.size()*random_nums2.size());
  std::cout << "Average time for " << random_nums1.size()*random_nums2.size() << " addition of two numbers: " << ave_diff << " ns" << std::endl;

  // lambda call
  start = clock();
  for (auto num1 : random_nums1)
    for (auto num2 : random_nums2)
      loop_sum += _adder(num1, num2);
  end = clock();
  ave_diff = duration <double, std::nano> (end - start).count() / static_cast<decltype(ave_diff)>(random_nums1.size()*random_nums2.size());
  std::cout << "Average time for " << random_nums1.size()*random_nums2.size() << " calls to (generic lambda) _adder: " << ave_diff << " ns" << std::endl;

  // curriable-wrapped function call
  start = clock();
  for (auto num1 : random_nums1)
    for (auto num2 : random_nums2)
      loop_sum += adder(num1, num2)();
  end = clock();
  ave_diff = duration <double, std::nano> (end - start).count() / static_cast<decltype(ave_diff)>(random_nums1.size()*random_nums2.size());
  std::cout << "Average time for " << random_nums1.size()*random_nums2.size() << " calls to (2 arg curried function) adder: " << ave_diff << " ns" << std::endl;

  // curriable-wrapped function call
  std::cout << std::endl << "Generic lambda (2 arguments reduced to 1 argument)" << std::endl;
  auto addtwo = adder(2);
  std::cout << "Value check: " << _adder(2, 3) << " == " << addtwo(3)() << std::endl;
  std::cout << "Are the stored function type and temporary type the same? : " << std::boolalpha << std::is_same<decltype(addtwo), decltype(adder(2))>::value << std::endl;

  std::cout << "Priming loops ..." << std::endl;
  for (auto num1 : random_nums1)
    loop_sum += addtwo(num1)();

  start = clock();
  for (auto num1 : random_nums1)
    loop_sum += addtwo(num1)();
  end = clock();
  ave_diff = duration <double, std::nano> (end - start).count() / static_cast<decltype(ave_diff)>(random_nums1.size());
  std::cout << "Average time for " << random_nums1.size() << " calls to (1 arg curried function) addtwo: " << ave_diff << " ns" << std::endl;



  // 3 argument generic lambda with placeholders
  std::cout << std::endl << "Generic lambda (3 arguments)" << std::endl;
  auto addtrio = make_curriable<3>([](auto x1, auto x2, auto x3) {return x1 + x2 + x2 + x3 + x3 + x3;});
  auto _addtrio = [](auto x1, auto x2, auto x3) {return x1 + x2 + x2 + x3 + x3 + x3;};
  std::cout << "Value check: " << _addtrio(2,3,4) << " == " << addtrio(2,3,4)() << " == " << addtrio(2,3)(4)() << " == " << addtrio(2)(3)(4)() << std::endl;

  std::cout << "Priming loops ..." << std::endl;
  for (auto num1 : random_nums1)
    for (auto num2 : random_nums2)
      for (auto num3 : random_nums3)
        loop_sum += num1 + num2 + num2 + num3 + num3 + num3;

  // no function call
  start = clock();
  for (auto num1 : random_nums1)
    for (auto num2 : random_nums2)
      for (auto num3 : random_nums3)
        loop_sum += num1 + num2 + num2 + num3 + num3 + num3;
  end = clock();
  ave_diff = duration <double, std::nano> (end - start).count() / static_cast<decltype(ave_diff)>(random_nums1.size()*random_nums2.size()*random_nums3.size());
  std::cout << "Average time for " << random_nums1.size()*random_nums2.size()*random_nums3.size() << " addition of three numbers: " << ave_diff << " ns" << std::endl;

  // lambda call
  start = clock();
  for (auto num1 : random_nums1)
    for (auto num2 : random_nums2)
      for (auto num3 : random_nums3)
        loop_sum += _addtrio(num1, num2, num3);
  end = clock();
  ave_diff = duration <double, std::nano> (end - start).count() / static_cast<decltype(ave_diff)>(random_nums1.size()*random_nums2.size()*random_nums3.size());
  std::cout << "Average time for " << random_nums1.size()*random_nums2.size()*random_nums3.size() << " calls to (lambda function) _addtrio: " << ave_diff << " ns" << std::endl;

  // curriable-wrapped function call
  start = clock();
  for (auto num1 : random_nums1)
    for (auto num2 : random_nums2)
      for (auto num3 : random_nums3)
        loop_sum += addtrio(num1, num2, num3)();
  end = clock();
  ave_diff = duration <double, std::nano> (end - start).count() / static_cast<decltype(ave_diff)>(random_nums1.size()*random_nums2.size()*random_nums3.size());
  std::cout << "Average time for " << random_nums1.size()*random_nums2.size()*random_nums3.size() << " calls to (3 arg curried function) addtrio: " << ave_diff << " ns" << std::endl;


  std::cout << std::endl << "Generic lambda (3 arguments, 2 placeholders)" << std::endl;
  auto add2p = addtrio(_,_,4);
  auto _add2p = [](auto&& x1, auto&& x2) {return x1 + x2 + x2 + 4 + 4 + 4;};
  std::cout << "Value check: " << _add2p(2,3) << " == " << add2p(2,3)() << " == " << add2p(2)(3)() << std::endl;

  std::cout << "Priming loops ..." << std::endl;
  for (auto num1 : random_nums1)
    for (auto num2 : random_nums2)
      loop_sum += num1 + num2;

  // lambda call
  start = clock();
  for (auto num1 : random_nums1)
    for (auto num2 : random_nums2)
      loop_sum += _add2p(num1, num2);
  end = clock();
  ave_diff = duration <double, std::nano> (end - start).count() / static_cast<decltype(ave_diff)>(random_nums1.size()*random_nums2.size());
  std::cout << "Average time for " << random_nums1.size()*random_nums2.size() << " calls to (lambda function) _add2p: " << ave_diff << " ns" << std::endl;

  // curriable-wrapped function call
  start = clock();
  for (auto num1 : random_nums1)
    for (auto num2 : random_nums2)
      loop_sum += add2p(num1, num2)();
  end = clock();
  ave_diff = duration <double, std::nano> (end - start).count() / static_cast<decltype(ave_diff)>(random_nums1.size()*random_nums2.size());
  std::cout << "Average time for " << random_nums1.size()*random_nums2.size() << " calls to (3 arg curried function, 2 placeholders) add2p: " << ave_diff << " ns" << std::endl;



  // composition
  std::cout << std::endl << "Composition (with * operator)" << std::endl;
  auto comp = addtwo * addtwo * addtwo * addtrio(2);
  auto manual_comp = [](auto a, auto b) {return 2 + 2*(2 + 2*(2 + 2*(2 + 2*a + 3*b)));};
  std::cout << "Value check: " << manual_comp(5, 3) << " == " << comp(5, 3)() << std::endl;

  std::cout << "Priming loops ..." << std::endl;
  for (auto num1 : random_nums1)
    for (auto num2 : random_nums2)
      loop_sum += comp(num1, num2)();

  // lambda call
  start = clock();
  for (auto num1 : random_nums1)
    for (auto num2 : random_nums2)
      loop_sum += manual_comp(num1, num2);
  end = clock();
  ave_diff = duration <double, std::nano> (end - start).count() / static_cast<decltype(ave_diff)>(random_nums1.size()*random_nums2.size());
  std::cout << "Average time for " << random_nums1.size()*random_nums2.size() << " calls to (generic lambda) manual_comp: " << ave_diff << " ns" << std::endl;

  // composed, curriable-wrapped function call
  start = clock();
  for (auto num1 : random_nums1)
    for (auto num2 : random_nums2)
      loop_sum += comp(num1, num2)();
  end = clock();
  ave_diff = duration <double, std::nano> (end - start).count() / static_cast<decltype(ave_diff)>(random_nums1.size()*random_nums2.size());
  std::cout << "Average time for " << random_nums1.size()*random_nums2.size() << " calls to (2 arg curried function) comp: " << ave_diff << " ns" << std::endl;



  // runtime performance
  std::cout << std::endl << "Storing in a std::function" << std::endl;
  auto re_eager = make_eager(addtwo);
  std::function<int(int)> re = addtwo;
  auto rere = make_curriable(re);
  auto rerere = make_curriable<decltype(addtwo.func)>(re); // i.e. rerere == addtwo
  // auto rerere = make_curriable(addtwo, re); // i.e. rerere == addtwo
  // re(3,4); // compile-time error
  // re(3)(); // compile-time error

  std::cout << "Value check: " << 2+3+3 << " == " << re(3) << std::endl;

  std::cout << "Priming loops ..." << std::endl;
  for (auto num1 : random_nums1)
    loop_sum += re(num1);

  // adding two numbers
  start = clock();
  for (auto num1 : random_nums1)
    loop_sum += 2 + num1 + num1;
  end = clock();
  ave_diff = duration <double, std::nano> (end - start).count() / static_cast<decltype(ave_diff)>(random_nums1.size());
  std::cout << "Average time for " << random_nums1.size() << " addition of two numbers: " << ave_diff << " ns" << std::endl;

  // re-eagered function
  start = clock();
  for (auto num1 : random_nums1)
    loop_sum += re_eager(num1);
  end = clock();
  ave_diff = duration <double, std::nano> (end - start).count() / static_cast<decltype(ave_diff)>(random_nums1.size());
  std::cout << "Average time for " << random_nums1.size() << " calls to (re-eagered function) re_eager: " << ave_diff << " ns" << std::endl;

  // std::function function
  start = clock();
  for (auto num1 : random_nums1)
    loop_sum += re(num1);
  end = clock();
  ave_diff = duration <double, std::nano> (end - start).count() / static_cast<decltype(ave_diff)>(random_nums1.size());
  std::cout << "Average time for " << random_nums1.size() << " calls to (1 arg std::function) re: " << ave_diff << " ns" << std::endl;

  // re-curried function
  start = clock();
  for (auto num1 : random_nums1)
    loop_sum += rere(num1)();
  end = clock();
  ave_diff = duration <double, std::nano> (end - start).count() / static_cast<decltype(ave_diff)>(random_nums1.size());
  std::cout << "Average time for " << random_nums1.size() << " calls to (1 arg re-curried function) rere: " << ave_diff << " ns" << std::endl;

  // re-curried function
  start = clock();
  for (auto num1 : random_nums1)
    loop_sum += rerere(num1)();
  end = clock();
  ave_diff = duration <double, std::nano> (end - start).count() / static_cast<decltype(ave_diff)>(random_nums1.size());
  std::cout << "Average time for " << random_nums1.size() << " calls to (1 arg re-curried \"type specified\" function) rerere: " << ave_diff << " ns" << std::endl;




  // thunk performance
  std::cout << std::endl << "Forcing and retrieving a thunk" << std::endl;
  std::list<std::function<int()>> random_thunks;
  for (auto num1 : random_nums1)
    // random_thunks.push_back(static_cast<std::function<int()>>(addtwo(num1))); // requires ugly static_cast
    random_thunks.emplace_back(addtwo(num1));

  std::cout << "Priming loops ..." << std::endl;
  for (auto num1 : random_nums1)
    loop_sum += num1;

  start = clock();
  for (auto num1 : random_nums1)
    loop_sum += num1;
  end = clock();
  ave_diff = duration <double, std::nano> (end - start).count() / static_cast<decltype(ave_diff)>(random_nums1.size());
  std::cout << "Average time for " << random_nums1.size() << " additions of random numbers: " << ave_diff << " ns" << std::endl;

  std::cout << "Priming thunk loops ..." << std::endl;
  for (const auto &thunk : random_thunks) // ignore this warning
    loop_sum += 1;

  start = clock();
  for (const auto &thunk : random_thunks)
    loop_sum += thunk();
  end = clock();
  ave_diff = duration <double, std::nano> (end - start).count() / static_cast<decltype(ave_diff)>(random_nums1.size());
  std::cout << "Average time for " << random_nums1.size() << " calls to force a thunk: " << ave_diff << " ns" << std::endl;

  start = clock();
  for (const auto &thunk : random_thunks)
    loop_sum += thunk();
  end = clock();
  ave_diff = duration <double, std::nano> (end - start).count() / static_cast<decltype(ave_diff)>(random_nums1.size());
  std::cout << "Average time for " << random_nums1.size() << " calls to retrieve a thunk: " << ave_diff << " ns" << std::endl;




  std::cout << "\n\nDummy sum value: " << loop_sum << std::endl;
  std::cout << "\n\ninfix test: " << 2 %adder% 3 << std::endl;
  std::cout << "call test: " << adder% 2% 3 << std::endl;

  // std::function is subtype polymorphic (but eager)!!!
  //  I think for the sake of maximal STL compatibility std::function should take the role of indirect functoids
  // struct A {};
  // struct B : public A {};
  // struct C {};
  // struct D : public C {};
  // std::function<D*(A*)> derived;
  // std::function<C*(B*)> base = derived;
  // base = [](A *) {return new D();};

  return 0;

  // // testing const-ness of lambda and arguments (passed!)
  // auto addeight = addtrio(2, 3);
  // std::cout << addtrio(2)(3)(4)() << ", " << addtrio(2)(3, 4)() << ", " << addeight(4)() << std::endl;
  // std::cout << addtrio(2)(3)(4, 5, 'd')() << std::endl;

  // auto add18 = addtrio(_, 3, 4);
  // auto addduo = addtrio(_, 3);
  // auto add14 = addtrio(2, _, 4);
  // std::cout << add18(2)() << ", "<< addduo(2,4)() << std::endl;
  // std::cout << add14(3)() << std::endl;
  // int arg = 3;
  // std::cout << add14(arg)() << std::endl;
  // const int &arg_ref = arg;
  // std::cout << add14(arg_ref)() << std::endl;
  // int &arg_ref2 = arg;
  // ++arg_ref2;
  // std::cout << add14(arg_ref2)() << std::endl;

  // // std::cout << "is thunk forced?" << std::endl;
  // auto addtriothunk = addtrio(2)(3)(4);
  // std::cout << addtriothunk() << std::endl;
  // std::cout << addtriothunk() << std::endl;
  // 
  // std::function<int()> holder = addtrio(2)(3)(4);
  // std::cout << holder() << ", " << holder() << std::endl;
  // holder = addtwo(3);
  // std::cout << holder() << ", " << holder() << std::endl;


  // // std::cout << "is thunk forced?" << std::endl;
  // auto addtriothunk = addtrio(2)(3)(4);
  // std::cout << addtriothunk() << std::endl;
  // std::cout << addtriothunk() << std::endl;

  // const auto& l = [](auto x) {return 2.0*x;};
  // auto c = make_curriable<1>(l);
  // std::cout << l(5) << ", " << c(5)() << std::endl;

  // auto factorial = fix([](auto&& self, long double n) -> long double
  //     { return n < 2.0 ? 1.0 : n * self(self, n - 1.0); });

  // std::cout << factorial(1000) << std::endl;
}
