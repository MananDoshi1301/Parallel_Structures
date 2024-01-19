mysort: main.cpp
	g++ main.cpp -pthread -O3 -std=c++2a -g -o mysort

mysort_eg: 
	./mysort -i case1.txt -o your1.txt -t 5 --alg=merge

merge_par: merge_par.cpp
	g++ merge_par.cpp -pthread -O3 -std=c++2a -g -o merge_par

sample: sample.cpp
	g++ sample.cpp -pthread -O3 -std=c++2a -g -o sample


.PHONY : clean
clean:
	rm mysort merge_par merge_par1 sample 
