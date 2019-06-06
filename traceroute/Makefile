CC=g++

traceroute: main.o checksum.o receive.o
	$(CC) -o traceroute main.o checksum.o receive.o


main.o: main.cpp checksum.h traceroute.h
	$(CC) -c -std=c++11 -Wall -Wpedantic -Wextra -Wconversion -Wshadow main.cpp

checksum.o: checksum.cpp checksum.h traceroute.h
	$(CC) -c -std=c++11 -Wall -Wpedantic -Wextra -Wconversion -Wshadow checksum.cpp

receive.o: receive.cpp receive.h traceroute.h
	$(CC) -c -std=c++11 -Wall -Wpedantic -Wextra -Wconversion -Wshadow receive.cpp


clean:
	rm -r ./*.o ./traceroute