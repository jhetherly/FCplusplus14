#ifndef FCPP_FUNCTOID_H
#define FCPP_FUNCTOID_H

#include <utility>
#include <type_traits>
#include <functional>
// without any thread safety for the thunk, execution with 
//  O2 level optimization is identical to hand coded function bodies
//  (i.e. the compiler can see through all the function calls)
#if defined(FCPP_TREADSAFE_SUSP)
#include <mutex>  // increases execution by 30x
#endif

namespace fcpp {

// forward helper functions
template <class F, int N> struct curried_type;
template <int N, class F> auto make_curriable (F &&f);
template <int N, class F> auto make_eager (const curried_type<F,N> &c);



// recursive lambda helper
template<typename Functor>
struct fix_type {
  Functor functor;

  template<typename... Args>
  decltype(auto) operator()(Args&&... args) const &
  { return functor(functor, std::forward<Args>(args)...); }

  template<typename... Args>
  decltype(auto) operator()(Args&&... args) &&
  { return functor(std::move(functor), std::forward<Args>(args)...); }
};

template<typename Functor>
fix_type<typename std::decay<Functor>::type> fix(Functor&& functor)
{ return { std::forward<Functor>(functor) }; }

// // allows for the following usage
// auto factorial = fix([](auto&& self, int n) -> int
// { return n < 2 ? 1 : n * self(self, n - 1); });
// 
// assert( factorial(5) == 120 )



// placeholder for curried functions
struct placeholder {};
const placeholder _{};



template <class F, int N>
struct curried_type {
  using func_type = F;
  F func;

  curried_type() = delete;
  curried_type (F &&f) : func(std::move(f)) {}
  curried_type (const F &f) : func(f) {}

  // automatic conversion to std::function
  template <class T, class ...Args>
  operator std::function<T(Args...)>() const &
  {
    std::function<T(Args...)> temp{F(func)};
    return temp; 
  }
  template <class T, class ...Args>
  operator std::function<T(Args...)>() &&
  {
    std::function<T(Args...)> temp{std::move(func)};
    return temp; 
  }
  

  // normal currying
  template <class Arg1, class ...Args, 
           typename std::enable_if<!std::is_same<typename std::decay<Arg1>::type, placeholder>::value, int>::type = 0>
  auto operator() (Arg1&& a1, Args&& ...args) const &
  {
    const F &f = func; // make locally scoped for copying into lambda
    auto temp = make_curriable<N - 1>(
        [a1 = std::forward<Arg1>(a1), f](auto&& ...an) 
        {return f(a1, std::forward<decltype(an)>(an)...);}
        )(std::forward<Args>(args)...);
    return temp;
  }
  template <class Arg1, class ...Args, 
           typename std::enable_if<!std::is_same<typename std::decay<Arg1>::type, placeholder>::value, int>::type = 0>
  auto operator() (Arg1&& a1, Args&& ...args) &&
  {
    const F &f = func; // make locally scoped for copying into lambda
    auto temp = make_curriable<N - 1>(
        [a1 = std::forward<Arg1>(a1), f = std::move(f)](auto&& ...an) 
        {return f(a1, std::forward<decltype(an)>(an)...);}
        )(std::forward<Args>(args)...);
    return temp;
  }

  // placeholder currying
  template <class Arg1, class ...Args, 
           typename std::enable_if<std::is_same<typename std::decay<Arg1>::type, placeholder>::value, int>::type = 0>
  auto operator() (Arg1&&, Args&& ...args) const &
  {
    auto temp = make_curriable<N>(F(func)).template new_function<1>(std::forward<Args>(args)...);
    return temp;
  }
  template <class Arg1, class ...Args, 
           typename std::enable_if<std::is_same<typename std::decay<Arg1>::type, placeholder>::value, int>::type = 0>
  auto operator() (Arg1&&, Args&& ...args) &&
  {
    auto temp = make_curriable<N>(std::move(func)).template new_function<1>(std::forward<Args>(args)...);
    return temp;
  }

  // placeholder currying (no placeholder)
  // NN == 1
  template <int NN, class Arg1, class ...Args, 
           typename std::enable_if<!std::is_same<typename std::decay<Arg1>::type, placeholder>::value, int>::type = 0,
           typename std::enable_if<NN == 1, int>::type = 0>
  auto new_function (Arg1&& a1, Args&& ...args) const &
  {
    const F &f = func; // make local for copying into lambda
    auto temp = make_curriable<N - 1>(
        [a1 = std::forward<Arg1>(a1), f](auto&& p1, auto&& ...an) 
        {return f(std::forward<decltype(p1)>(p1), 
          a1, std::forward<decltype(an)>(an)...);}
        ).template new_function<NN>(std::forward<Args>(args)...);
    return temp;
  }
  template <int NN, class Arg1, class ...Args, 
           typename std::enable_if<!std::is_same<typename std::decay<Arg1>::type, placeholder>::value, int>::type = 0,
           typename std::enable_if<NN == 1, int>::type = 0>
  auto new_function (Arg1&& a1, Args&& ...args) &&
  {
    const F &f = func; // make local for copying into lambda
    auto temp = make_curriable<N - 1>(
        [a1 = std::forward<Arg1>(a1), f = std::move(f)](auto&& p1, auto&& ...an) 
        {return f(std::forward<decltype(p1)>(p1), 
          a1, std::forward<decltype(an)>(an)...);}
        ).template new_function<NN>(std::forward<Args>(args)...);
    return temp;
  }
  // NN == 2
  template <int NN, class Arg1, class ...Args, 
           typename std::enable_if<!std::is_same<typename std::decay<Arg1>::type, placeholder>::value, int>::type = 0,
           typename std::enable_if<NN == 2, int>::type = 0>
  auto new_function (Arg1&& a1, Args&& ...args) const &
  {
    const F &f = func; // make local for copying into lambda
    auto temp = make_curriable<N - 1>(
        [a1 = std::forward<Arg1>(a1), f](auto&& p1, auto&& p2, auto&& ...an) 
        {return f(std::forward<decltype(p1)>(p1), 
          std::forward<decltype(p2)>(p2), 
          a1, std::forward<decltype(an)>(an)...);}
        ).template new_function<NN>(std::forward<Args>(args)...);
    return temp;
  }
  template <int NN, class Arg1, class ...Args, 
           typename std::enable_if<!std::is_same<typename std::decay<Arg1>::type, placeholder>::value, int>::type = 0,
           typename std::enable_if<NN == 2, int>::type = 0>
  auto new_function (Arg1&& a1, Args&& ...args) &&
  {
    const F &f = func; // make local for copying into lambda
    auto temp = make_curriable<N - 1>(
        [a1 = std::forward<Arg1>(a1), f = std::move(f)](auto&& p1, auto&& p2, auto&& ...an) 
        {return f(std::forward<decltype(p1)>(p1), 
          std::forward<decltype(p2)>(p2), 
          a1, std::forward<decltype(an)>(an)...);}
        ).template new_function<NN>(std::forward<Args>(args)...);
    return temp;
  }
  // NN == 3
  template <int NN, class Arg1, class ...Args, 
           typename std::enable_if<!std::is_same<typename std::decay<Arg1>::type, placeholder>::value, int>::type = 0,
           typename std::enable_if<NN == 3, int>::type = 0>
  auto new_function (Arg1&& a1, Args&& ...args) const
  {
    const F &f = func; // make local for copying into lambda
    auto temp = make_curriable<N - 1>(
        [a1 = std::forward<Arg1>(a1), f](auto&& p1, auto&& p2, auto&& p3, auto&& ...an) 
        {return f(std::forward<decltype(p1)>(p1), 
          std::forward<decltype(p2)>(p2), 
          std::forward<decltype(p3)>(p3), 
          a1, std::forward<decltype(an)>(an)...);}
        ).template new_function<NN>(std::forward<Args>(args)...);
    return temp;
  }
  // NN == 4
  template <int NN, class Arg1, class ...Args, 
           typename std::enable_if<!std::is_same<typename std::decay<Arg1>::type, placeholder>::value, int>::type = 0,
           typename std::enable_if<NN == 4, int>::type = 0>
  auto new_function (Arg1&& a1, Args&& ...args) const &
  {
    const F &f = func; // make local for copying into lambda
    auto temp = make_curriable<N - 1>(
        [a1 = std::forward<Arg1>(a1), f](auto&& p1, auto&& p2, auto&& p3, auto&& p4, auto&& ...an) 
        {return f(std::forward<decltype(p1)>(p1), 
          std::forward<decltype(p2)>(p2), 
          std::forward<decltype(p3)>(p3), 
          std::forward<decltype(p4)>(p4), 
          a1, std::forward<decltype(an)>(an)...);}
        ).template new_function<NN>(std::forward<Args>(args)...);
    return temp;
  }
  template <int NN, class Arg1, class ...Args, 
           typename std::enable_if<!std::is_same<typename std::decay<Arg1>::type, placeholder>::value, int>::type = 0,
           typename std::enable_if<NN == 4, int>::type = 0>
  auto new_function (Arg1&& a1, Args&& ...args) &&
  {
    const F &f = func; // make local for copying into lambda
    auto temp = make_curriable<N - 1>(
        [a1 = std::forward<Arg1>(a1), f = std::move(f)](auto&& p1, auto&& p2, auto&& p3, auto&& p4, auto&& ...an) 
        {return f(std::forward<decltype(p1)>(p1), 
          std::forward<decltype(p2)>(p2), 
          std::forward<decltype(p3)>(p3), 
          std::forward<decltype(p4)>(p4), 
          a1, std::forward<decltype(an)>(an)...);}
        ).template new_function<NN>(std::forward<Args>(args)...);
    return temp;
  }
  // NN == 5
  template <int NN, class Arg1, class ...Args, 
           typename std::enable_if<!std::is_same<typename std::decay<Arg1>::type, placeholder>::value, int>::type = 0,
           typename std::enable_if<NN == 5, int>::type = 0>
  auto new_function (Arg1&& a1, Args&& ...args) const &
  {
    const F &f = func; // make local for copying into lambda
    auto temp = make_curriable<N - 1>(
        [a1 = std::forward<Arg1>(a1), f](auto&& p1, auto&& p2, auto&& p3, auto&& p4, auto&& p5, auto&& ...an) 
        {return f(std::forward<decltype(p1)>(p1), 
          std::forward<decltype(p2)>(p2), 
          std::forward<decltype(p3)>(p3), 
          std::forward<decltype(p4)>(p4), 
          std::forward<decltype(p5)>(p5), 
          a1, std::forward<decltype(an)>(an)...);}
        ).template new_function<NN>(std::forward<Args>(args)...);
    return temp;
  }
  template <int NN, class Arg1, class ...Args, 
           typename std::enable_if<!std::is_same<typename std::decay<Arg1>::type, placeholder>::value, int>::type = 0,
           typename std::enable_if<NN == 5, int>::type = 0>
  auto new_function (Arg1&& a1, Args&& ...args) &&
  {
    const F &f = func; // make local for copying into lambda
    auto temp = make_curriable<N - 1>(
        [a1 = std::forward<Arg1>(a1), f = std::move(f)](auto&& p1, auto&& p2, auto&& p3, auto&& p4, auto&& p5, auto&& ...an) 
        {return f(std::forward<decltype(p1)>(p1), 
          std::forward<decltype(p2)>(p2), 
          std::forward<decltype(p3)>(p3), 
          std::forward<decltype(p4)>(p4), 
          std::forward<decltype(p5)>(p5), 
          a1, std::forward<decltype(an)>(an)...);}
        ).template new_function<NN>(std::forward<Args>(args)...);
    return temp;
  }
  // NN == 6
  template <int NN, class Arg1, class ...Args, 
           typename std::enable_if<!std::is_same<typename std::decay<Arg1>::type, placeholder>::value, int>::type = 0,
           typename std::enable_if<NN == 6, int>::type = 0>
  auto new_function (Arg1&& a1, Args&& ...args) const &
  {
    const F &f = func; // make local for copying into lambda
    auto temp = make_curriable<N - 1>(
        [a1 = std::forward<Arg1>(a1), f](auto&& p1, auto&& p2, auto&& p3, auto&& p4, auto&& p5, auto&& p6, auto&& ...an) 
        {return f(std::forward<decltype(p1)>(p1), 
          std::forward<decltype(p2)>(p2), 
          std::forward<decltype(p3)>(p3), 
          std::forward<decltype(p4)>(p4), 
          std::forward<decltype(p5)>(p5), 
          std::forward<decltype(p6)>(p6), 
          a1, std::forward<decltype(an)>(an)...);}
        ).template new_function<NN>(std::forward<Args>(args)...);
    return temp;
  }
  template <int NN, class Arg1, class ...Args, 
           typename std::enable_if<!std::is_same<typename std::decay<Arg1>::type, placeholder>::value, int>::type = 0,
           typename std::enable_if<NN == 6, int>::type = 0>
  auto new_function (Arg1&& a1, Args&& ...args) &&
  {
    const F &f = func; // make local for copying into lambda
    auto temp = make_curriable<N - 1>(
        [a1 = std::forward<Arg1>(a1), f = std::move(f)](auto&& p1, auto&& p2, auto&& p3, auto&& p4, auto&& p5, auto&& p6, auto&& ...an) 
        {return f(std::forward<decltype(p1)>(p1), 
          std::forward<decltype(p2)>(p2), 
          std::forward<decltype(p3)>(p3), 
          std::forward<decltype(p4)>(p4), 
          std::forward<decltype(p5)>(p5), 
          std::forward<decltype(p6)>(p6), 
          a1, std::forward<decltype(an)>(an)...);}
        ).template new_function<NN>(std::forward<Args>(args)...);
    return temp;
  }
  // NN == 7
  template <int NN, class Arg1, class ...Args, 
           typename std::enable_if<!std::is_same<typename std::decay<Arg1>::type, placeholder>::value, int>::type = 0,
           typename std::enable_if<NN == 7, int>::type = 0>
  auto new_function (Arg1&& a1, Args&& ...args) const &
  {
    const F &f = func; // make local for copying into lambda
    auto temp = make_curriable<N - 1>(
        [a1 = std::forward<Arg1>(a1), f](auto&& p1, auto&& p2, auto&& p3, auto&& p4, auto&& p5, auto&& p6, auto&& p7, auto&& ...an) 
        {return f(std::forward<decltype(p1)>(p1), 
          std::forward<decltype(p2)>(p2), 
          std::forward<decltype(p3)>(p3), 
          std::forward<decltype(p4)>(p4), 
          std::forward<decltype(p5)>(p5), 
          std::forward<decltype(p6)>(p6), 
          std::forward<decltype(p7)>(p7), 
          a1, std::forward<decltype(an)>(an)...);}
        ).template new_function<NN>(std::forward<Args>(args)...);
    return temp;
  }
  template <int NN, class Arg1, class ...Args, 
           typename std::enable_if<!std::is_same<typename std::decay<Arg1>::type, placeholder>::value, int>::type = 0,
           typename std::enable_if<NN == 7, int>::type = 0>
  auto new_function (Arg1&& a1, Args&& ...args) &&
  {
    const F &f = func; // make local for copying into lambda
    auto temp = make_curriable<N - 1>(
        [a1 = std::forward<Arg1>(a1), f = std::move(f)](auto&& p1, auto&& p2, auto&& p3, auto&& p4, auto&& p5, auto&& p6, auto&& p7, auto&& ...an) 
        {return f(std::forward<decltype(p1)>(p1), 
          std::forward<decltype(p2)>(p2), 
          std::forward<decltype(p3)>(p3), 
          std::forward<decltype(p4)>(p4), 
          std::forward<decltype(p5)>(p5), 
          std::forward<decltype(p6)>(p6), 
          std::forward<decltype(p7)>(p7), 
          a1, std::forward<decltype(an)>(an)...);}
        ).template new_function<NN>(std::forward<Args>(args)...);
    return temp;
  }
  // NN == 8
  template <int NN, class Arg1, class ...Args, 
           typename std::enable_if<!std::is_same<typename std::decay<Arg1>::type, placeholder>::value, int>::type = 0,
           typename std::enable_if<NN == 8, int>::type = 0>
  auto new_function (Arg1&& a1, Args&& ...args) const &
  {
    const F &f = func; // make local for copying into lambda
    auto temp = make_curriable<N - 1>(
        [a1 = std::forward<Arg1>(a1), f](auto&& p1, auto&& p2, auto&& p3, auto&& p4, auto&& p5, auto&& p6, auto&& p7, auto&& p8, auto&& ...an) 
        {return f(std::forward<decltype(p1)>(p1), 
          std::forward<decltype(p2)>(p2), 
          std::forward<decltype(p3)>(p3), 
          std::forward<decltype(p4)>(p4), 
          std::forward<decltype(p5)>(p5), 
          std::forward<decltype(p6)>(p6), 
          std::forward<decltype(p7)>(p7), 
          std::forward<decltype(p8)>(p8), 
          a1, std::forward<decltype(an)>(an)...);}
        ).template new_function<NN>(std::forward<Args>(args)...);
    return temp;
  }
  template <int NN, class Arg1, class ...Args, 
           typename std::enable_if<!std::is_same<typename std::decay<Arg1>::type, placeholder>::value, int>::type = 0,
           typename std::enable_if<NN == 8, int>::type = 0>
  auto new_function (Arg1&& a1, Args&& ...args) &&
  {
    const F &f = func; // make local for copying into lambda
    auto temp = make_curriable<N - 1>(
        [a1 = std::forward<Arg1>(a1), f = std::move(f)](auto&& p1, auto&& p2, auto&& p3, auto&& p4, auto&& p5, auto&& p6, auto&& p7, auto&& p8, auto&& ...an) 
        {return f(std::forward<decltype(p1)>(p1), 
          std::forward<decltype(p2)>(p2), 
          std::forward<decltype(p3)>(p3), 
          std::forward<decltype(p4)>(p4), 
          std::forward<decltype(p5)>(p5), 
          std::forward<decltype(p6)>(p6), 
          std::forward<decltype(p7)>(p7), 
          std::forward<decltype(p8)>(p8), 
          a1, std::forward<decltype(an)>(an)...);}
        ).template new_function<NN>(std::forward<Args>(args)...);
    return temp;
  }

  // placeholder currying (with placeholder)
  template <int NN, class Arg1, class ...Args, typename std::enable_if<std::is_same<typename std::decay<Arg1>::type, placeholder>::value, int>::type = 0>
  auto new_function (Arg1&&, Args&& ...args) const &
  {
    auto temp = make_curriable<N>(F(func)).template new_function<NN + 1>(std::forward<Args>(args)...);
    return temp;
  }
  template <int NN, class Arg1, class ...Args, typename std::enable_if<std::is_same<typename std::decay<Arg1>::type, placeholder>::value, int>::type = 0>
  auto new_function (Arg1&&, Args&& ...args) &&
  {
    auto temp = make_curriable<N>(std::move(func)).template new_function<NN + 1>(std::forward<Args>(args)...);
    return temp;
  }

  template <int>
  auto new_function () const &
  {
    auto temp = make_curriable<N>(F(func));
    return temp;
  }
  template <int>
  auto new_function () &&
  {
    auto temp = make_curriable<N>(std::move(func));
    return temp;
  }

  auto operator() () const &
  {
    auto temp = make_curriable<N>(F(func));
    return temp;
  }
  auto operator() () &&
  {
    auto temp = make_curriable<N>(std::move(func));
    return temp;
  }
};




template <class F>
struct curried_type<F, 1> {
  using func_type = F;
  F func;

  curried_type() = delete;
  curried_type (F &&f) : func(std::move(f)) {}
  curried_type (const F &f) : func(f) {}

  // automatic conversion to std::function
  template <class T, class Arg>
  operator std::function<T(Arg)>() const &
  {
    std::function<T(Arg)> temp{F(func)};
    return temp; 
  }
  template <class T, class Arg>
  operator std::function<T(Arg)>() &&
  {
    std::function<T(Arg)> temp{std::move(func)};
    return temp; 
  }

  
  // normal currying
  template <class Arg1, class ...Args, 
           typename std::enable_if<!std::is_same<typename std::decay<Arg1>::type, placeholder>::value, int>::type = 0>
  auto operator() (Arg1&& a1, Args&& ...args) const &
  {
    const F &f = func; // make local for copying into lambda
    auto temp = make_curriable<0>(
        [a1 = std::forward<Arg1>(a1), f](auto&& ...an) 
        {return f(a1, std::forward<decltype(an)>(an)...);}
        );
    return temp;
  }
  template <class Arg1, class ...Args, 
           typename std::enable_if<!std::is_same<typename std::decay<Arg1>::type, placeholder>::value, int>::type = 0>
  auto operator() (Arg1&& a1, Args&& ...args) &&
  {
    const F &f = func; // make local for copying into lambda
    auto temp = make_curriable<0>(
        [a1 = std::forward<Arg1>(a1), f = std::move(f)](auto&& ...an) 
        {return f(a1, std::forward<decltype(an)>(an)...);}
        );
    return temp;
  }

  // placeholder currying
  template <class Arg1, class ...Args, 
           typename std::enable_if<std::is_same<typename std::decay<Arg1>::type, placeholder>::value, int>::type = 0>
  auto operator() (Arg1&&, Args&& ...args) const &
  {
    auto temp = make_curriable<1>(F(func)).template new_function<1>(std::forward<Args>(args)...);
    return temp;
  }
  template <class Arg1, class ...Args, 
           typename std::enable_if<std::is_same<typename std::decay<Arg1>::type, placeholder>::value, int>::type = 0>
  auto operator() (Arg1&&, Args&& ...args) &&
  {
    auto temp = make_curriable<1>(std::move(func)).template new_function<1>(std::forward<Args>(args)...);
    return temp;
  }

  // placeholder currying (no placeholder)
  // NN == 1
  template <int NN, class Arg1, class ...Args, 
           typename std::enable_if<!std::is_same<typename std::decay<Arg1>::type, placeholder>::value, int>::type = 0,
           typename std::enable_if<NN == 1, int>::type = 0>
  auto new_function (Arg1&& a1, Args&& ...args) const &
  {
    const F &f = func; // make local for copying into lambda
    auto temp = make_curriable<0>(
        [a1 = std::forward<Arg1>(a1), f](auto&& p1, auto&& ...an) 
        {return f(std::forward<decltype(p1)>(p1), 
          a1, std::forward<decltype(an)>(an)...);}
        ).template new_function<NN>(std::forward<Args>(args)...);
    return temp;
  }
  template <int NN, class Arg1, class ...Args, 
           typename std::enable_if<!std::is_same<typename std::decay<Arg1>::type, placeholder>::value, int>::type = 0,
           typename std::enable_if<NN == 1, int>::type = 0>
  auto new_function (Arg1&& a1, Args&& ...args) &&
  {
    const F &f = func; // make local for copying into lambda
    auto temp = make_curriable<0>(
        [a1 = std::forward<Arg1>(a1), f = std::move(f)](auto&& p1, auto&& ...an) 
        {return f(std::forward<decltype(p1)>(p1), 
          a1, std::forward<decltype(an)>(an)...);}
        ).template new_function<NN>(std::forward<Args>(args)...);
    return temp;
  }
  // NN == 2
  template <int NN, class Arg1, class ...Args, 
           typename std::enable_if<!std::is_same<typename std::decay<Arg1>::type, placeholder>::value, int>::type = 0,
           typename std::enable_if<NN == 2, int>::type = 0>
  auto new_function (Arg1&& a1, Args&& ...args) const &
  {
    const F &f = func; // make local for copying into lambda
    auto temp = make_curriable<0>(
        [a1 = std::forward<Arg1>(a1), f](auto&& p1, auto&& p2, auto&& ...an) 
        {return f(std::forward<decltype(p1)>(p1), 
          std::forward<decltype(p2)>(p2), 
          a1, std::forward<decltype(an)>(an)...);}
        ).template new_function<NN>(std::forward<Args>(args)...);
    return temp;
  }
  template <int NN, class Arg1, class ...Args, 
           typename std::enable_if<!std::is_same<typename std::decay<Arg1>::type, placeholder>::value, int>::type = 0,
           typename std::enable_if<NN == 2, int>::type = 0>
  auto new_function (Arg1&& a1, Args&& ...args) &&
  {
    const F &f = func; // make local for copying into lambda
    auto temp = make_curriable<0>(
        [a1 = std::forward<Arg1>(a1), f = std::move(f)](auto&& p1, auto&& p2, auto&& ...an) 
        {return f(std::forward<decltype(p1)>(p1), 
          std::forward<decltype(p2)>(p2), 
          a1, std::forward<decltype(an)>(an)...);}
        ).template new_function<NN>(std::forward<Args>(args)...);
    return temp;
  }
  // NN == 3
  template <int NN, class Arg1, class ...Args, 
           typename std::enable_if<!std::is_same<typename std::decay<Arg1>::type, placeholder>::value, int>::type = 0,
           typename std::enable_if<NN == 3, int>::type = 0>
  auto new_function (Arg1&& a1, Args&& ...args) const &
  {
    const F &f = func; // make local for copying into lambda
    auto temp = make_curriable<0>(
        [a1 = std::forward<Arg1>(a1), f](auto&& p1, auto&& p2, auto&& p3, auto&& ...an) 
        {return f(std::forward<decltype(p1)>(p1), 
          std::forward<decltype(p2)>(p2), 
          std::forward<decltype(p3)>(p3), 
          a1, std::forward<decltype(an)>(an)...);}
        ).template new_function<NN>(std::forward<Args>(args)...);
    return temp;
  }
  template <int NN, class Arg1, class ...Args, 
           typename std::enable_if<!std::is_same<typename std::decay<Arg1>::type, placeholder>::value, int>::type = 0,
           typename std::enable_if<NN == 3, int>::type = 0>
  auto new_function (Arg1&& a1, Args&& ...args) &&
  {
    const F &f = func; // make local for copying into lambda
    auto temp = make_curriable<0>(
        [a1 = std::forward<Arg1>(a1), f = std::move(f)](auto&& p1, auto&& p2, auto&& p3, auto&& ...an) 
        {return f(std::forward<decltype(p1)>(p1), 
          std::forward<decltype(p2)>(p2), 
          std::forward<decltype(p3)>(p3), 
          a1, std::forward<decltype(an)>(an)...);}
        ).template new_function<NN>(std::forward<Args>(args)...);
    return temp;
  }
  // NN == 4
  template <int NN, class Arg1, class ...Args, 
           typename std::enable_if<!std::is_same<typename std::decay<Arg1>::type, placeholder>::value, int>::type = 0,
           typename std::enable_if<NN == 4, int>::type = 0>
  auto new_function (Arg1&& a1, Args&& ...args) const &
  {
    const F &f = func; // make local for copying into lambda
    auto temp = make_curriable<0>(
        [a1 = std::forward<Arg1>(a1), f](auto&& p1, auto&& p2, auto&& p3, auto&& p4, auto&& ...an) 
        {return f(std::forward<decltype(p1)>(p1), 
          std::forward<decltype(p2)>(p2), 
          std::forward<decltype(p3)>(p3), 
          std::forward<decltype(p4)>(p4), 
          a1, std::forward<decltype(an)>(an)...);}
        ).template new_function<NN>(std::forward<Args>(args)...);
    return temp;
  }
  template <int NN, class Arg1, class ...Args, 
           typename std::enable_if<!std::is_same<typename std::decay<Arg1>::type, placeholder>::value, int>::type = 0,
           typename std::enable_if<NN == 4, int>::type = 0>
  auto new_function (Arg1&& a1, Args&& ...args) &&
  {
    const F &f = func; // make local for copying into lambda
    auto temp = make_curriable<0>(
        [a1 = std::forward<Arg1>(a1), f = std::move(f)](auto&& p1, auto&& p2, auto&& p3, auto&& p4, auto&& ...an) 
        {return f(std::forward<decltype(p1)>(p1), 
          std::forward<decltype(p2)>(p2), 
          std::forward<decltype(p3)>(p3), 
          std::forward<decltype(p4)>(p4), 
          a1, std::forward<decltype(an)>(an)...);}
        ).template new_function<NN>(std::forward<Args>(args)...);
    return temp;
  }
  // NN == 5
  template <int NN, class Arg1, class ...Args, 
           typename std::enable_if<!std::is_same<typename std::decay<Arg1>::type, placeholder>::value, int>::type = 0,
           typename std::enable_if<NN == 5, int>::type = 0>
  auto new_function (Arg1&& a1, Args&& ...args) const &
  {
    const F &f = func; // make local for copying into lambda
    auto temp = make_curriable<0>(
        [a1 = std::forward<Arg1>(a1), f](auto&& p1, auto&& p2, auto&& p3, auto&& p4, auto&& p5, auto&& ...an) 
        {return f(std::forward<decltype(p1)>(p1), 
          std::forward<decltype(p2)>(p2), 
          std::forward<decltype(p3)>(p3), 
          std::forward<decltype(p4)>(p4), 
          std::forward<decltype(p5)>(p5), 
          a1, std::forward<decltype(an)>(an)...);}
        ).template new_function<NN>(std::forward<Args>(args)...);
    return temp;
  }
  template <int NN, class Arg1, class ...Args, 
           typename std::enable_if<!std::is_same<typename std::decay<Arg1>::type, placeholder>::value, int>::type = 0,
           typename std::enable_if<NN == 5, int>::type = 0>
  auto new_function (Arg1&& a1, Args&& ...args) &&
  {
    const F &f = func; // make local for copying into lambda
    auto temp = make_curriable<0>(
        [a1 = std::forward<Arg1>(a1), f = std::move(f)](auto&& p1, auto&& p2, auto&& p3, auto&& p4, auto&& p5, auto&& ...an) 
        {return f(std::forward<decltype(p1)>(p1), 
          std::forward<decltype(p2)>(p2), 
          std::forward<decltype(p3)>(p3), 
          std::forward<decltype(p4)>(p4), 
          std::forward<decltype(p5)>(p5), 
          a1, std::forward<decltype(an)>(an)...);}
        ).template new_function<NN>(std::forward<Args>(args)...);
    return temp;
  }
  // NN == 6
  template <int NN, class Arg1, class ...Args, 
           typename std::enable_if<!std::is_same<typename std::decay<Arg1>::type, placeholder>::value, int>::type = 0,
           typename std::enable_if<NN == 6, int>::type = 0>
  auto new_function (Arg1&& a1, Args&& ...args) const &
  {
    const F &f = func; // make local for copying into lambda
    auto temp = make_curriable<0>(
        [a1 = std::forward<Arg1>(a1), f](auto&& p1, auto&& p2, auto&& p3, auto&& p4, auto&& p5, auto&& p6, auto&& ...an) 
        {return f(std::forward<decltype(p1)>(p1), 
          std::forward<decltype(p2)>(p2), 
          std::forward<decltype(p3)>(p3), 
          std::forward<decltype(p4)>(p4), 
          std::forward<decltype(p5)>(p5), 
          std::forward<decltype(p6)>(p6), 
          a1, std::forward<decltype(an)>(an)...);}
        ).template new_function<NN>(std::forward<Args>(args)...);
    return temp;
  }
  template <int NN, class Arg1, class ...Args, 
           typename std::enable_if<!std::is_same<typename std::decay<Arg1>::type, placeholder>::value, int>::type = 0,
           typename std::enable_if<NN == 6, int>::type = 0>
  auto new_function (Arg1&& a1, Args&& ...args) &&
  {
    const F &f = func; // make local for copying into lambda
    auto temp = make_curriable<0>(
        [a1 = std::forward<Arg1>(a1), f = std::move(f)](auto&& p1, auto&& p2, auto&& p3, auto&& p4, auto&& p5, auto&& p6, auto&& ...an) 
        {return f(std::forward<decltype(p1)>(p1), 
          std::forward<decltype(p2)>(p2), 
          std::forward<decltype(p3)>(p3), 
          std::forward<decltype(p4)>(p4), 
          std::forward<decltype(p5)>(p5), 
          std::forward<decltype(p6)>(p6), 
          a1, std::forward<decltype(an)>(an)...);}
        ).template new_function<NN>(std::forward<Args>(args)...);
    return temp;
  }
  // NN == 7
  template <int NN, class Arg1, class ...Args, 
           typename std::enable_if<!std::is_same<typename std::decay<Arg1>::type, placeholder>::value, int>::type = 0,
           typename std::enable_if<NN == 7, int>::type = 0>
  auto new_function (Arg1&& a1, Args&& ...args) const &
  {
    const F &f = func; // make local for copying into lambda
    auto temp = make_curriable<0>(
        [a1 = std::forward<Arg1>(a1), f](auto&& p1, auto&& p2, auto&& p3, auto&& p4, auto&& p5, auto&& p6, auto&& p7, auto&& ...an) 
        {return f(std::forward<decltype(p1)>(p1), 
          std::forward<decltype(p2)>(p2), 
          std::forward<decltype(p3)>(p3), 
          std::forward<decltype(p4)>(p4), 
          std::forward<decltype(p5)>(p5), 
          std::forward<decltype(p6)>(p6), 
          std::forward<decltype(p7)>(p7), 
          a1, std::forward<decltype(an)>(an)...);}
        ).template new_function<NN>(std::forward<Args>(args)...);
    return temp;
  }
  template <int NN, class Arg1, class ...Args, 
           typename std::enable_if<!std::is_same<typename std::decay<Arg1>::type, placeholder>::value, int>::type = 0,
           typename std::enable_if<NN == 7, int>::type = 0>
  auto new_function (Arg1&& a1, Args&& ...args) &&
  {
    const F &f = func; // make local for copying into lambda
    auto temp = make_curriable<0>(
        [a1 = std::forward<Arg1>(a1), f = std::move(f)](auto&& p1, auto&& p2, auto&& p3, auto&& p4, auto&& p5, auto&& p6, auto&& p7, auto&& ...an) 
        {return f(std::forward<decltype(p1)>(p1), 
          std::forward<decltype(p2)>(p2), 
          std::forward<decltype(p3)>(p3), 
          std::forward<decltype(p4)>(p4), 
          std::forward<decltype(p5)>(p5), 
          std::forward<decltype(p6)>(p6), 
          std::forward<decltype(p7)>(p7), 
          a1, std::forward<decltype(an)>(an)...);}
        ).template new_function<NN>(std::forward<Args>(args)...);
    return temp;
  }
  // NN == 8
  template <int NN, class Arg1, class ...Args, 
           typename std::enable_if<!std::is_same<typename std::decay<Arg1>::type, placeholder>::value, int>::type = 0,
           typename std::enable_if<NN == 8, int>::type = 0>
  auto new_function (Arg1&& a1, Args&& ...args) const &
  {
    const F &f = func; // make local for copying into lambda
    auto temp = make_curriable<0>(
        [a1 = std::forward<Arg1>(a1), f](auto&& p1, auto&& p2, auto&& p3, auto&& p4, auto&& p5, auto&& p6, auto&& p7, auto&& p8, auto&& ...an) 
        {return f(std::forward<decltype(p1)>(p1), 
          std::forward<decltype(p2)>(p2), 
          std::forward<decltype(p3)>(p3), 
          std::forward<decltype(p4)>(p4), 
          std::forward<decltype(p5)>(p5), 
          std::forward<decltype(p6)>(p6), 
          std::forward<decltype(p7)>(p7), 
          std::forward<decltype(p8)>(p8), 
          a1, std::forward<decltype(an)>(an)...);}
        ).template new_function<NN>(std::forward<Args>(args)...);
    return temp;
  }
  template <int NN, class Arg1, class ...Args, 
           typename std::enable_if<!std::is_same<typename std::decay<Arg1>::type, placeholder>::value, int>::type = 0,
           typename std::enable_if<NN == 8, int>::type = 0>
  auto new_function (Arg1&& a1, Args&& ...args) &&
  {
    const F &f = func; // make local for copying into lambda
    auto temp = make_curriable<0>(
        [a1 = std::forward<Arg1>(a1), f = std::move(f)](auto&& p1, auto&& p2, auto&& p3, auto&& p4, auto&& p5, auto&& p6, auto&& p7, auto&& p8, auto&& ...an) 
        {return f(std::forward<decltype(p1)>(p1), 
          std::forward<decltype(p2)>(p2), 
          std::forward<decltype(p3)>(p3), 
          std::forward<decltype(p4)>(p4), 
          std::forward<decltype(p5)>(p5), 
          std::forward<decltype(p6)>(p6), 
          std::forward<decltype(p7)>(p7), 
          std::forward<decltype(p8)>(p8), 
          a1, std::forward<decltype(an)>(an)...);}
        ).template new_function<NN>(std::forward<Args>(args)...);
    return temp;
  }

  // placeholder currying (with placeholder)
  template <int NN, class Arg1, class ...Args, typename std::enable_if<std::is_same<typename std::decay<Arg1>::type, placeholder>::value, int>::type = 0>
  auto new_function (Arg1&&, Args&& ...args) const &
  {
    auto temp = make_curriable<1>(F(func)).template new_function<NN + 1>(std::forward<Args>(args)...);
    return temp;
  }
  template <int NN, class Arg1, class ...Args, typename std::enable_if<std::is_same<typename std::decay<Arg1>::type, placeholder>::value, int>::type = 0>
  auto new_function (Arg1&&, Args&& ...args) &&
  {
    auto temp = make_curriable<1>(std::move(func)).template new_function<NN + 1>(std::forward<Args>(args)...);
    return temp;
  }

  template <int>
  auto new_function () const &
  {
    auto temp = make_curriable<1>(F(func));
    return temp;
  }
  template <int>
  auto new_function () &&
  {
    auto temp = make_curriable<1>(std::move(func));
    return temp;
  }

  auto operator() () const &
  {
    auto temp = make_curriable<1>(F(func));
    return temp;
  }
  auto operator() () &&
  {
    auto temp = make_curriable<1>(std::move(func));
    return temp;
  }
};




// suspension (result of func() should be either copy- or move-constructable)
template <class F>
struct curried_type<F, 0> {
  using func_type = F;
#if defined(FCPP_TREADSAFE_SUSP)
  mutable std::once_flag once_flag;
#endif

  F func;

  using result_type = typename std::decay<decltype(func())>::type;
  mutable typename std::aligned_storage<sizeof(result_type), alignof(result_type)>::type result;

  using thunk_type = result_type const & (*) (const curried_type<F, 0>*);
  mutable thunk_type thunk;

  // thunk
  static result_type const & thunkForce (const curried_type<F, 0> *susp)
  {
    return susp->setMemo();
  }
  // thunk
  static result_type const & thunkGet (const curried_type<F, 0> *susp)
  {
    return susp->getMemo();
  }

  result_type const & getMemo () const
  {
    return reinterpret_cast<result_type&>(result);
  }

#if defined(FCPP_TREADSAFE_SUSP)
  void setMemo_impl () const
  {
    new(&result) result_type(func());
    thunk = &thunkGet;
  }
#endif

  result_type const & setMemo () const
  {
#if defined(FCPP_TREADSAFE_SUSP)
    std::call_once(once_flag, [this]() {this->setMemo_impl();});
#else
    // should be same as setMemo_impl()
    new(&result) result_type(func());
    thunk = &thunkGet;
#endif
    return this->getMemo();
  }

  curried_type() = delete;
#if defined(FCPP_TREADSAFE_SUSP)
  // needed for mutex approach
  curried_type (curried_type &&c) : func(std::move(c.func)), thunk(&thunkForce) {}
  curried_type (const curried_type &c) : func(c.func), thunk(&thunkForce) {}
#else
  curried_type (curried_type &&c) = default;
  curried_type (const curried_type &c) = default;
#endif
  ~curried_type() 
  {
    if (thunk == &thunkGet) 
      reinterpret_cast<result_type&>(result).~result_type();
  }

  curried_type (F &&f) : func(std::move(f)), thunk(&thunkForce) {}
  curried_type (const F &f) : func(f), thunk(&thunkForce) {}

  template <class ...Args>
  auto operator() (Args&& ...) const &
  {
    return thunk(this);
  }
  template <class ...Args>
  auto operator() (Args&& ...) &&
  {
    // no need to memoize value if temporary
    return func();
  }
};




template <int N, class F>
auto make_curriable (F &&f)
{
  curried_type<typename std::decay<F>::type, N> temp(std::forward<F>(f));
  return temp;
}

template <class Ret, class ...Args>
auto make_curriable (Ret (*f)(Args ...args))
{
  curried_type<typename std::decay<decltype(f)>::type, sizeof...(Args)> temp(f);
  return temp;
}

template <class F, class Ret, class T, class ...Args>
auto make_curriable (F &&f, Ret (T::*)(Args ...args) const)
{
  curried_type<typename std::decay<F>::type, sizeof...(Args)> temp(std::forward<F>(f));
  return temp;
}

// this is incredibly inefficient, but may be necessary in some cases
template <class Ret, class ...Args>
auto make_curriable (const std::function<Ret(Args...)> &f)
{
  curried_type<typename std::decay<decltype(f)>::type, sizeof...(Args)> temp(f);
  return temp;
}

// try to use either this or the next one instead
//  this one needs the explicit type
template <class F, class Ret, class ...Args>
auto make_curriable (const std::function<Ret(Args...)> &f)
{
  auto ptr_func = *f.template target<F>();
  curried_type<typename std::decay<F>::type, sizeof...(Args)> temp(ptr_func);
  return temp;
}

//  this one needs a "template" curried_type (beware! if original function was a lambda this function effectually returns c)
template <class F, int N, class Ret, class ...Args>
auto make_curriable (const curried_type<F,N> &c, const std::function<Ret(Args...)> &f)
{
  auto ptr_func = *f.template target<F>();
  curried_type<typename std::decay<F>::type, sizeof...(Args)> temp(ptr_func);
  return temp;
}



// ////////////////////////////////////////////
// create a suspension to manage a single value
// ////////////////////////////////////////////
template <class T>
auto make_suspension_for_value (T&& val)
{
  auto temp = make_curriable<0>([val = std::forward<T>(val)]() {return val;});
  new(&temp.result) typename std::decay<T>::type(std::forward<T>(val));
  temp.thunk = &decltype(temp)::thunkGet;
  return temp;
}



// ////////////////////
// composition operator
// ////////////////////
template <class F1, class F2, int N2>
auto operator< (const curried_type<F1, 1> &c1, const curried_type<F2, N2> &c2)
{
  auto temp = make_curriable<N2>([c1, c2](auto&& ...args) {return c1(c2(std::forward<decltype(args)>(args)...)())();});
  return temp;
}
template <class F1, class F2, int N2>
auto operator< (curried_type<F1, 1>&& c1, const curried_type<F2, N2> &c2)
{
  auto temp = make_curriable<N2>([c1 = std::move(c1), c2](auto&& ...args) {return c1(c2(std::forward<decltype(args)>(args)...)())();});
  return temp;
}
template <class F1, class F2, int N2>
auto operator< (const curried_type<F1, 1> &c1, curried_type<F2, N2>&& c2)
{
  auto temp = make_curriable<N2>([c1, c2 = std::move(c2)](auto&& ...args) {return c1(c2(std::forward<decltype(args)>(args)...)())();});
  return temp;
}
template <class F1, class F2, int N2>
auto operator< (curried_type<F1, 1>&& c1, curried_type<F2, N2>&& c2)
{
  auto temp = make_curriable<N2>([c1 = std::move(c1), c2 = std::move(c2)](auto&& ...args) {return c1(c2(std::forward<decltype(args)>(args)...)())();});
  return temp;
}



// ///////////////////////////////////////////////////////
// allows for eager usage without casting to std::function
// ///////////////////////////////////////////////////////
template <int N, class F>
auto make_eager (curried_type<F,N> &&c)
{
  return [c = std::move(c)](auto&& ...args) {
    static_assert(sizeof...(args) == N, "supplied the incorrect number of arguments to eager function"); 
    return c(std::forward<decltype(args)>(args)...)();};
}

template <int N, class F>
auto make_eager (const curried_type<F,N> &c)
{
  return [&c](auto&& ...args) {
    static_assert(sizeof...(args) == N, "supplied the incorrect number of arguments to eager function");
    return c(std::forward<decltype(args)>(args)...)();};
}


}

#endif
