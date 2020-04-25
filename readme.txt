brief:
the aim of this project is to simulate a switch router: it has 3 sockets which get incoming messages.
the 3 sockets route the messages to 3 mapped sockets(each source socket is mapped to a destination
socket. the src socket sends messages to the destination socket and vice versa.

how to the project.
no need to compile: just fire up the application in command line : ./switch
then select from "configfile.txt", source ip and port, and destination ip and port(each line
represents src ip,src port, dest ip,dest port, respectively).
open new terminal and write the command: netcat -l $(dest port).
open another terminal and write the command: telnet $(src ip)  $(src port).
then begin to send messages from source teminal to dest terminal and vice versa.


if you wanna make changes: make changes, and then run "make clean && make" command.


if you want to see logs printed out, go the debug.txt file and just write in the
first line the word 'yes'.
