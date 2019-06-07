#include "common.h"
#include "packets.h"
#include "nodes.h"
#include "progbar.h"
#include "filter.h"

#define WINDOW_SIZE 3000

size_t cirb_start = 0;
size_t cirb_csize = 0;

pkt_node nodez[WINDOW_SIZE];


int main(int argc, char const *argv[])
{
	if(argc != 5)
		errx(ERR_USAGE, "Usage: %s <IP> <port> <filename> <size>\n", argv[0]);

	struct sockaddr_in server_address = {0};
	server_address.sin_family = AF_INET;

	if(!inet_pton(AF_INET, argv[1], &server_address.sin_addr))
		errx(ERR_INVALID_IP, "Invalid IP address!\n");

	uint16_t in_sin_port;
	if(sscanf(argv[2], "%hu", &in_sin_port) <= 0)
		errx(ERR_INVALID_PORT, "Invalid port number!\n");

	server_address.sin_port = htons(in_sin_port);


	int out_fd = open(argv[3],
		O_APPEND | O_TRUNC | O_CREAT | O_NDELAY | O_WRONLY | O_CLOEXEC,
		S_IRUSR | S_IWUSR );

	if(out_fd == -1)
		err(ERR_OPEN, "open() failed" );

	const uintmax_t in_total_size = strtoumax(argv[4], NULL, 0);

	if(in_total_size == 0 || errno == ERANGE)
		err(ERR_INVALID_SIZE, "Invalid size!\n");


	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	if (sockfd < 0)
		err(ERR_SOCKET, "socket() failed");


	attach_addr_filter(sockfd, &server_address);
	// // or, alternative implementation
	// if(connect(sockfd, (struct sockaddr*)&server_address, sizeof(server_address))<0)
		// err(ERR_CONNECT, "connect() failed");


	{
		struct timeval timeout;
		timeout.tv_sec = 1;
		timeout.tv_usec = 0;
		setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
	}	

	size_t cstart = 0;
	size_t size_left = in_total_size;
	size_t file_cursor = 0;

	char recvbuf[IP_MAXPACKET+1];


	while(in_total_size - file_cursor > 0)
	{

		struct timeval now;
		gettimeofday(&now, NULL);

		while(cirb_csize != 0 && 
			nodez[(cirb_start) % WINDOW_SIZE].received)
		{

			if(write(out_fd, nodez[cirb_start].data, nodez[cirb_start].size) !=
				 (int)nodez[cirb_start].size)
				err(ERR_WRITE, "write(out_fd) failed");

			file_cursor += nodez[cirb_start].size;

			nodez[cirb_start].received = false;

			cirb_csize--;
			cirb_start = (cirb_start + 1) % WINDOW_SIZE;
		}

		while(cirb_csize < WINDOW_SIZE && size_left > 0)
		{
			size_t cpid = (cirb_start + cirb_csize) % WINDOW_SIZE;
			nodez[cpid].start = cstart;
			nodez[cpid].size  = (size_left < 1000) ? size_left : 1000;
			nodez[cpid].received = false;
			nodez[cpid].chance = 0;
			nodez[cpid].lastsent = now;

			cstart    += nodez[cpid].size; 
			size_left -= nodez[cpid].size;

			cirb_csize++;
		}

		for (size_t cpoff = 0; cpoff < cirb_csize; ++cpoff)
		{
			size_t cpid = (cpoff + cirb_start) % WINDOW_SIZE;
			if(!nodez[cpid].received)
			{
				struct timeval diff;
				struct timeval deadline;
				timersub(&now, &nodez[cpid].lastsent, &diff);

				deadline.tv_sec = 0;
				deadline.tv_usec = 1000000 / (nodez[cpid].chance+1);

				if(nodez[cpid].chance !=0 && timercmp(&diff, &deadline, <))
					continue;

				send_get(sockfd, (const struct sockaddr*) &server_address,
					&nodez[(cpoff + cirb_start) % WINDOW_SIZE]);

				nodez[cpid].lastsent = now;
				nodez[cpid].chance++;
			}

		}

		ssize_t recvsize = recv(sockfd, recvbuf, IP_MAXPACKET, 0);

		if(recvsize < 0 )
		{
			if ((errno != EAGAIN) && (errno != EWOULDBLOCK) && (errno != EINTR))
				err(ERR_RECV, "recvfrom() failed");
		}
		else
		{
			uintmax_t pkt_start; 
			uintmax_t pkt_size; 
			uintmax_t real_pkt_size; 
			char* payload_ptr = parse_packet(recvbuf, (size_t)recvsize, 
				&pkt_start, &pkt_size, &real_pkt_size);

			if(payload_ptr != NULL && pkt_size == real_pkt_size)
			{
				size_t calc_idx = (pkt_start - file_cursor) / 1000;
				size_t cpid = (cirb_start + calc_idx)%WINDOW_SIZE;

				if(calc_idx < cirb_csize &&	
					nodez[cpid].start == pkt_start && nodez[cpid].size  == pkt_size)
				{
					memcpy(nodez[cpid].data, payload_ptr, pkt_size);
					nodez[cpid].received = true;
				}
			}


		}

		prog_tick();
		render_progbar((int)file_cursor, (int)in_total_size);
		fflush(stdout);
	}


	puts("");


	if(close(sockfd))
		err(ERR_CLOSE_SOCK, "close(sockfd) failed");

	if(close(out_fd))
		err(ERR_CLOSE_OUTFD, "close(out_fd) failed");

	return 0;
}