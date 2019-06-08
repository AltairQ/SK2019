#ifndef GET
#define GET

#include <stdbool.h>

typedef struct get_hdr {
	char path[256];
	char host[256];
	bool conn_close;
} get_hdr;


get_hdr* parse_get_packet(char* buf, size_t len);

#endif	