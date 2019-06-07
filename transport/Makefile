CC=gcc

transport: main.o packets.o progbar.o nodes.o filter.o
	$(CC) -o transport main.o packets.o progbar.o nodes.o filter.o


main.o: main.c packets.h common.h nodes.h progbar.h filter.h
	$(CC) -c -std=gnu99 -Wall -Wpedantic -Wextra -Wconversion -Wshadow main.c

packets.o: packets.c packets.h common.h
	$(CC) -c -std=gnu99 -Wall -Wpedantic -Wextra -Wconversion -Wshadow packets.c

nodes.o: nodes.c common.h nodes.h
	$(CC) -c -std=gnu99 -Wall -Wpedantic -Wextra -Wconversion -Wshadow nodes.c

progbar.o: progbar.c common.h progbar.h
	$(CC) -c -std=gnu99 -Wall -Wpedantic -Wextra -Wconversion -Wshadow progbar.c

filter.o: filter.c common.h filter.h
	$(CC) -c -std=gnu99 -Wall -Wpedantic -Wextra -Wconversion -Wshadow filter.c


clean:
	rm -f ./*.o

distclean: clean
	rm -f ./transport