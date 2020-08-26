switch: main.o server.o
	g++ main.o server.o   -pthread -o switch


main.o:  main.cpp server.h
	g++ -c main.cpp

server.o:  server.cpp server.h  singletone.h port.h MessageRouter.h
	g++   -c server.cpp


clean:
	rm -rf *.o switch
