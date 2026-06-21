all: server client

server: server.c
	gcc -Wall -Wextra -o server server.c

client: client.c
	gcc -Wall -Wextra -o client client.c

clean:
	rm -f server client
