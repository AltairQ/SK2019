#include "common.h"
#include "get.h"
#include "response.h"
#include "mime.h"

#define BUFFSIZE 10000000

char buffer[BUFFSIZE+1];
char pathbuffer[PATH_MAX];
char hostpathbuffer[PATH_MAX];

int main(int argc, const char** argv)
{

	if(argc != 3)
	{
		fprintf(stderr, "Usage: %s <port> <html dir>\n", argv[0]);
		return -1;
	}

	unsigned short bind_port;
	if(sscanf(argv[1], "%hu", &bind_port) != 1)	
		ERROR("Invalid port!");

	const char* html_dir = argv[2];

	{
		struct stat hdir_stat;
		if(stat(html_dir, &hdir_stat)== -1)
			ERROR("Error opening html directory!");

		if(!S_ISDIR(hdir_stat.st_mode))
			ERROR("Error opening html directory - not a directory!");
	}
	

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);	
	if (sockfd < 0)
		ERROR("socket error");

	{
		int reuseport = 1;
		if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &reuseport, sizeof(reuseport)) < 0)
			fprintf(stderr, "SO_REUSEPORT failed, ignoring\n");
	}

	struct sockaddr_in server_address = {0};
	server_address.sin_family      = AF_INET;
	server_address.sin_port        = htons(bind_port);
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind (sockfd, (struct sockaddr*) &server_address, sizeof(server_address)) < 0) 
		ERROR("bind error");

	if (listen (sockfd, 64) < 0)
		ERROR("listen error");

	for (;;)
	{
		int conn_sockfd = accept (sockfd, NULL, NULL);
		if (conn_sockfd < 0)
			ERROR("accept error")

		struct timeval timeout;
		timeout.tv_sec = 1;
		timeout.tv_usec = 0;

		while(true)
		{

			if(setsockopt(conn_sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0)
				ERROR("SO_RCVTIMEO failed");

			ssize_t n = recv(conn_sockfd, buffer, BUFFSIZE, 0);

			if (n < 0)
			{
				if ((errno != EAGAIN) && (errno != EWOULDBLOCK))				
					fprintf(stderr, "recv error");

				break;
			}
				
			if(n == 0)
				break;


			size_t msglen = (size_t)n;
			buffer[n] = 0;


			get_hdr *tmp = parse_get_packet(buffer, msglen);

			if(tmp != NULL)
			{
				fprintf(stderr, "received GET %s %s\t", tmp->path, tmp->host);
			}
			else
			{
				send_501(conn_sockfd);
				fprintf(stderr, "すきじゃない (malformed/not implemented)\n");
				goto later_exit;
			}

			timeout.tv_sec = tmp->conn_close ? 0 : 1;

			if(strstr(tmp->path, "/../") != NULL)
			{
				send_403(conn_sockfd);
				fprintf(stderr, "-> 403\n");
				goto later_exit;
			}

			// Path calculations
			snprintf(pathbuffer, PATH_MAX, "%s/%s%s",
					html_dir,
					tmp->host, tmp->path);

			snprintf(hostpathbuffer, PATH_MAX, "%s/%s",
					html_dir,
					tmp->host); // we guarantee host is free from '.' and '/'

			char* req_realpath = realpath(pathbuffer, NULL);
			char* host_root = realpath(hostpathbuffer,NULL);

			if(req_realpath == NULL || host_root == NULL)
			{
				send_404(conn_sockfd);
				fprintf(stderr, "-> 404\n");
				goto exit;
			}

			if(strstr(req_realpath, host_root) != req_realpath)
			{
				send_403(conn_sockfd);
				fprintf(stderr, "-> 403\n");
				goto exit;
			}

			struct stat reqf_stat;

			if(stat(req_realpath, &reqf_stat) < 0)
			{
				send_404(conn_sockfd);
				fprintf(stderr, "-> 404\n");
				goto exit;
			}

			if(S_ISDIR(reqf_stat.st_mode))
			{
				snprintf(pathbuffer, PATH_MAX, "http://%s:%hu%sindex.html",
					tmp->host,
					bind_port,
					tmp->path);
				send_301(conn_sockfd, pathbuffer);
				fprintf(stderr, "-> 301 (%s)\n", pathbuffer);
				goto exit;
			}

			send_response_path(conn_sockfd, req_realpath);
			fprintf(stderr, "-> 200\n");

		exit:
			free(req_realpath);
			free(host_root);

		later_exit:
			free(tmp);
		}

		if (close (conn_sockfd) < 0)
			ERROR("close error");

		
	}
}
