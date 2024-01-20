all: mysort counter

exampleCPP: exampleCPP.cpp
	g++ exampleCPP.cpp -pthread -O3 -std=c++2a -g -o exampleCPP

exampleC: exampleC.c
	gcc exampleC.c -pthread -O3 -g -o exampleC
    
tas: tas.cpp
	g++ tas.cpp -pthread -O3 -std=c++2a -g -o tas
    
test_tas: test_tas.cpp
	g++ test_tas.cpp -pthread -O3 -std=c++2a -g -o test_tas
    
ticket_lock: ticket_lock.cpp
	g++ ticket_lock.cpp -pthread -O3 -std=c++2a -g -o ticket_lock
    
MCS: MCS.cpp
	g++ MCS.cpp -pthread -O3 -std=c++2a -g -o MCS
    
sense: sense.cpp
	g++ sense.cpp -pthread -O3 -std=c++2a -g -o sense
    
mysort: buck_sort.cpp
	g++ buck_sort.cpp -pthread -O3 -std=c++2a -g -o mysort
    
try_file: try_file.cpp
	g++ try_file.cpp -pthread -O3 -std=c++2a -g -o try_file
    
counter: counter.cpp
	g++ counter.cpp -pthread -O3 -std=c++2a -g -o counter

.PHONY : clean
clean:
	rm exampleC exampleCPP mysort counter tas test_tas try_file ticket_lock MCS sense
