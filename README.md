# FCplusplus14
Reenvisioning of FC++ with C++14

# Introduction
The benefits of functional programming in an increasingly multithreaded
world are many, to say nothing of the increase in readability and
maintainability of source code. The addition of several paradigm shifting
features to C++ in the form of C++11 and C++14 have made this style of 
programming even more efficient and syntactically accessible than was 
previously possible with earlier versions of C++. FC++ is an excellent 
implementation of many Haskell concepts in C++98. However, it was written long 
before move semantics, generic lambdas, more robust type deduction, alignment 
specifications, and many other features C++14 brings to bear. For example, the 
machinery behind the direct and indirect functoids required a fare amount of 
setup that many would find inexcusable in light of C++'s new lambda support. 
Thus, FC++14 aims to bring FC++ in line with the recent advances in C++ 
technology and performance.

# Direct and Indirect Functoids
In the original FC++, functoids were an attempt to make C++ functions and 
functors more Haskell-like. This included features like currying and subtype
polymorphism. From a technical standpoint they also included support for 
FC++'s robust type system. This required a bit of setup from the creator of a 
functoid and has no support for the new lambdas in C++. Thus, to modern eyes 
it can look a bit clunky. Also, it only allowed for a finite number of
function parameters and couldn't always forward arguments correctly due to
language limitations. The solution FC++14 uses is to create a relatively 
flexible and thin wrapper around a given function, functor, or lambda that
overcomes all of these issues. This wrapper is equivalent to FC++'s direct
functoids. A purposeful choice was made to not support indirect functoids, and
instead defer to `std::function`.  
The reasons for this are:

* `std::function` already implements subtype polymorphism
* Full currying support is rarely needed for indirect functoids
* When currying is required with type erasure, the aforementioned wrapper can 
simply be used with `std::function`
* Type-erasing the thunk is the most common use for `std::function`, thus this
naturally leads to a curry-free scenario and this will still preserve the
return value suspension
