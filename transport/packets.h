#ifndef PACKETS_H
#define PACKETS_H

#include "nodes.h"

char* craft_get_packet(uintmax_t start, uintmax_t size, int* buf_size);
char* parse_packet(char* buf, size_t size, uintmax_t* start, uintmax_t* out_size, uintmax_t* real_size );
void send_get(int sockfd, const struct sockaddr* addr, const pkt_node* node);
#endif