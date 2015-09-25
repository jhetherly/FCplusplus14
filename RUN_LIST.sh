mkdir bin
rm -rf bin/list

#g++ -std=c++14 -I. -O$1 -Wall src/list.cpp -o bin/list
clang++ -std=c++14 -I. -DFCPP_TREADSAFE_SUSP -O$1 -Wall src/list.cpp -o bin/list
#g++ -std=c++14 -ftemplate-depth=1000 -I. -DFCPP_TREADSAFE_SUSP -O$1 -Wall src/list.cpp -o bin/list

./bin/list
