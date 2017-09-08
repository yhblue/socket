start:
	gcc -g -Wall -o client client.c message.pb-c.c -lprotobuf-c
	gcc -g -Wall -o server server.c

clean:
	rm -f client server