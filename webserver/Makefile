CC=gcc

webserver: main.o get.o response.o mime.o
	$(CC) -o webserver main.o get.o response.o mime.o

main.o: main.c common.h get.h response.h
	$(CC) -c -std=gnu99 -Wall -Wpedantic -Wextra -Wconversion -Wshadow main.c

get.o: get.c common.h get.h
	$(CC) -c -std=gnu99 -Wall -Wpedantic -Wextra -Wconversion -Wshadow get.c

response.o: response.c common.h response.h
	$(CC) -c -std=gnu99 -Wall -Wpedantic -Wextra -Wconversion -Wshadow response.c

mime.o: mime.c common.h mime.h
	$(CC) -c -std=gnu99 -Wall -Wpedantic -Wextra -Wconversion -Wshadow mime.c

clean:
	rm -f ./*.o

distclean: clean
	rm -f ./webserver