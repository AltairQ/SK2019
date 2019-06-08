#include "common.h"
#include "get.h"


get_hdr* parse_get_packet(char* buf, size_t len)
{
	get_hdr *tr = malloc(sizeof(get_hdr));

	char conn_buf[256] = {0};
	char host_buf[256] = {0};

	size_t i = 0;
	size_t cbeg = 0;

	while(i + 1 < len)
	{
		if(buf[i] == '\r' && buf[i+1] == '\n')
		{
			buf[i] = 0;
			buf[i+1] = 0;
			i+= 2;
			break;
		}
		i++;
	}

	if(sscanf((buf+cbeg), "GET %256s HTTP/1.1", tr->path) != 1)
	{
		// fprintf(stderr, "malformed GET header\n");
		// fprintf(stderr, "encountered: %s\n", (buf+cbeg));
		goto fail;
	}

	bool has_host = false;

	while(i + 1 < len)
	{
		cbeg = i;

		while(i + 1 < len)
		{
			if(buf[i] == '\r' && buf[i+1] == '\n')
			{
				buf[i] = 0;
				buf[i+1] = 0;
				i+= 2;
				break;
			}
			i++;
		}

		if(sscanf((buf+cbeg), "Host: %256s", host_buf ) == 1)
		{
			char* colon = strchr(host_buf, ':');
			if(colon != NULL)
				*colon = 0;

			if(strchr(host_buf, '.') != NULL || strchr(host_buf, '/') != NULL)
				goto fail;

			strcpy(tr->host, host_buf);

			has_host = true;
		}
		else
			if(sscanf((buf+cbeg), "Connection: %256s", (char*)conn_buf) == 1)
			{
				tr->conn_close = strcmp(conn_buf, "close") == 0;
			}
	}


	if(!has_host) goto fail;

	return tr;


	fail:
		free(tr);
		return NULL;


}


