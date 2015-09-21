mkdir bin
rm -rf bin/list

clang++ -std=c++14 -I. -O$1 -Wall src/list.cpp -o bin/list
#clang++ -std=c++14 -I. -DFCPP_TREADSAFE_SUSP -O$1 -Wall src/list.cpp -o bin/list

./bin/list
