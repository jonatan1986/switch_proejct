switch: main.o server.o
	# g++ main.o server.o  -o switch
	g++ main.o server.o -L.-libsingletone   -pthread -o switch

# main: main.o server.o signletone.o

# server: server.o signletone.o

main.o: # main.cpp server.cpp
	g++ -c main.cpp

server.o: # server.cpp  singletone.cpp singletone.h
	g++   -c  server.cpp


clean:
	rm -rf *.o switch
#	g++ -pthread main.cpp server.cpp bidirmap.cpp
