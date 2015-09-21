#include <iostream>
#include <random>
#include <chrono>

#include "FC++14/functiod.h"
#include "FC++14/list.h"


using namespace std::chrono;
using namespace fcpp;

int main ()
{
  List<int> l;
  cons(2, l);

  return 0;
}
