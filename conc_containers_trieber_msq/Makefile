all: sgl_stack sgl_queue tstack ms_queue coh tstack_elim num_elim sgl_elim

sgl_stack: sgl_stack.cpp
	g++ sgl_stack.cpp -pthread -O3 -std=c++2a -g -o sgl_stack

sgl_queue: sgl_queue.cpp
	g++ sgl_queue.cpp -pthread -O3 -std=c++2a -g -o sgl_queue

tstack: tstack.cpp
	g++ tstack.cpp -pthread -O3 -std=c++2a -g -o tstack

ms_queue: m_and_s_queue.cpp
	g++ m_and_s_queue.cpp -pthread -O3 -std=c++2a -g -o ms_queue

tstack_elim: tstack_elim.cpp
	g++ tstack_elim.cpp -pthread -O3 -std=c++2a -g -o tstack_elim

num_elim: num_elim.cpp
	g++ num_elim.cpp -pthread -O3 -std=c++2a -g -o num_elim

sgl_elim: sgl_elim.cpp
	g++ sgl_elim.cpp -pthread -O3 -std=c++2a -g -o sgl_elim

coh: coh.cpp
	g++ coh.cpp -pthread -O3 -std=c++2a -g -o coh

.PHONY : clean
clean:
	rm sgl_stack sgl_queue tstack ms_queue coh tstack_elim num_elim sgl_elim
