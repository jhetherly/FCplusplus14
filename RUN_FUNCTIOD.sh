clang++ -std=c++14 -I. -O$1 -Wall src/functiod.cpp -o functiod
#clang++ -std=c++14 -I. -DFCPP_TREADSAFE_SUSP -O$1 -Wall src/functiod.cpp -o functiod

./functiod
