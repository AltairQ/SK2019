#include <linux/filter.h>
#include "filter.h"

void attach_addr_filter(int sockfd, const struct sockaddr_in* server_address)
{
	uint16_t hi = (uint16_t)(ntohl(server_address->sin_addr.s_addr) >> 16);
	uint16_t lo = (uint16_t)(ntohl(server_address->sin_addr.s_addr) & 0xFFFF);

	struct sock_filter filter[] = {
		BPF_STMT(BPF_LD + BPF_H + BPF_ABS, (unsigned int)SKF_NET_OFF + 16),
		BPF_JUMP(BPF_JMP + BPF_JEQ + BPF_K, hi, 0, 3),       
		BPF_STMT(BPF_LD + BPF_H + BPF_ABS, (unsigned int)SKF_NET_OFF + 18),
		BPF_JUMP(BPF_JMP + BPF_JEQ + BPF_K, lo, 0, 1),       
		BPF_STMT(BPF_RET + BPF_K, 65535),                    
		BPF_STMT(BPF_LD + BPF_H + BPF_ABS, (unsigned int)SKF_NET_OFF + 12),
		BPF_JUMP(BPF_JMP + BPF_JEQ + BPF_K, hi, 0, 3),       
		BPF_STMT(BPF_LD + BPF_H + BPF_ABS, (unsigned int)SKF_NET_OFF + 14),
		BPF_JUMP(BPF_JMP + BPF_JEQ + BPF_K, lo, 0, 1),       
		BPF_STMT(BPF_RET + BPF_K, 65535),                    
		BPF_STMT(BPF_RET + BPF_K, 0)                         
	};

	struct sock_fprog fprog = {
		.len = sizeof(filter) / sizeof(filter[0]),
		.filter = filter
	};

	if(setsockopt(sockfd, SOL_SOCKET, SO_ATTACH_FILTER, &fprog, sizeof(fprog)))
		err(ERR_BPF, "setsockopt(SO_ATTACH_FILTER) failed");
}