main : main.o regexp.o
	g++ -o main main.o regexp.o

main.o : main.cpp regexp.hpp
	g++ -c main.cpp

regexp.o : regexp.hpp regexp.cpp
	g++ -c regexp.cpp

clean :
	rm *.o
	rm main