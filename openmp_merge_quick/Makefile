all: mysort
    
hello: hello.cpp
	g++ hello.cpp -pthread -O3 -std=c++2a -g -o hello -fopenmp

quick: quick.cpp
	g++ quick.cpp -pthread -O3 -std=c++2a -g -o quick -fopenmp

quick1: quick1.cpp
	g++ quick1.cpp -pthread -O3 -std=c++2a -g -o quick1 -fopenmp

mysort: mysort.cpp
	g++ mysort.cpp -pthread -O3 -std=c++2a -g -o mysort -fopenmp


.PHONY : clean
clean:
	rm mysort quick1 quick hello merge