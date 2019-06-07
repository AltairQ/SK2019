#ifndef COMMON_H
#define COMMON_H

#include <fcntl.h>
#include <inttypes.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdbool.h>
#include <err.h>

// return codes
#define ERR_USAGE			-(__COUNTER__+1)
#define ERR_INVALID_IP		-(__COUNTER__+1)
#define ERR_INVALID_PORT	-(__COUNTER__+1)
#define ERR_INVALID_SIZE	-(__COUNTER__+1)
#define ERR_OPEN			-(__COUNTER__+1)
#define ERR_CLOSE_OUTFD		-(__COUNTER__+1)
#define ERR_SOCKET			-(__COUNTER__+1)
#define ERR_SENDTO			-(__COUNTER__+1)
#define ERR_BPF				-(__COUNTER__+1)
#define ERR_CLOSE_SOCK		-(__COUNTER__+1)
#define ERR_RECV			-(__COUNTER__+1)
#define ERR_WRITE			-(__COUNTER__+1)
#define ERR_CONNECT			-(__COUNTER__+1)

#endif