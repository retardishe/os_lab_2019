all: client server

dbg: client-dbg server-dbg

client-dbg: client.c libhelp
	gcc -o client-dbg.out client.c -I./libhelp -L./ -lpthread -lm -lhelp -g -O0

server-dbg: server.c libhelp
	gcc -o server-dbg.out  server.c -I./libhelp -L./ -lpthread -lm -lhelp -g -O0

client: client.c libhelp
	gcc -o client.out client.c -I./libhelp -L./ -lpthread -lm -lhelp

server: server.c libhelp
	gcc -o server.out server.c -I./libhelp -L./ -lpthread -lm -lhelp

libhelp: ./libhelp/help.c
	gcc -o ./libhelp/libhelp.o -c ./libhelp/help.c -I./libhelp
	ar rcs ./libhelp.a ./libhelp/libhelp.o

clean:
	rm -r *.o *.a *.out -f