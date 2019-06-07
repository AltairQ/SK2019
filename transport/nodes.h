#ifndef NODES_H
#define NODES_H

typedef struct pkt_node {
	uintmax_t start;
	uintmax_t size;
	char data[1024];
	uint8_t chance;
	struct timeval lastsent;
	bool received;
} pkt_node;

#endif