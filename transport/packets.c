#include "common.h"
#include "packets.h"

static char getpkt_buf[1024];

char* craft_get_packet(uintmax_t start, uintmax_t size, int* buf_size)
{
	*buf_size = snprintf(getpkt_buf, 0x20, "GET %lu %lu\n", start, size);
	return (char*)getpkt_buf;
}


void send_get(int sockfd, const struct sockaddr* addr, const pkt_node* node)
{
	int outpkt_size;
	const char* getpkt = craft_get_packet(node->start, node->size, &outpkt_size);

	if(sendto(sockfd, getpkt, (size_t)outpkt_size, 0, addr, sizeof(struct sockaddr))
		!= outpkt_size)
		err(ERR_SENDTO, "send() failed");
}



char* parse_packet(char* buf, size_t size, 
	uintmax_t* start, uintmax_t* out_size, uintmax_t* real_size )
{
	size_t newline_offset = 0;

	for(size_t i = 0; i < size; i++)
	{
		if(buf[i] == '\n')
		{
			newline_offset = i;
			buf[i] = 0;
			break;
		}
	}

	if(newline_offset == 0)
		return NULL;

	if(sscanf(buf, "DATA %lu %lu", start, out_size) != 2)
		return NULL;

	*real_size = size - newline_offset - 1;

	return buf + newline_offset + 1;	

}