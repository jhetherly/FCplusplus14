mkdir bin
rm -rf bin/functoid

clang++ -std=c++14 -I. -O$1 -Wall src/functoid.cpp -o bin/functoid
#clang++ -std=c++14 -I. -DFCPP_TREADSAFE_SUSP -O$1 -Wall src/functoid.cpp -o bin/functoid

./bin/functoid
