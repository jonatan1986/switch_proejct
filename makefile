switch: main.o server.o
	# g++ main.o server.o  -o switch
	g++ main.o server.o  -pthread -o switch

main.o:
	g++ -c main.cpp

server.o:
	g++   -c  server.cpp

clean:
	rm -rf *.o switch
#	g++ -pthread main.cpp server.cpp bidirmap.cpp
