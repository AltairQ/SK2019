#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>


#include "traceroute.h"
#include "checksum.h"
#include "ping_response.h"
#include "receive.h"

int main(int argc, char const *argv[])
{

	if (argc != 2)
	{
		std::cerr << "Usage: " << std::string(argv[0]) << " <destination IP>\n";
		exit(-__COUNTER__);
	}

	if (geteuid() != 0)
	{
		std::cerr << "Run it as root.\n";
		exit(-__COUNTER__);
	}


	// Prepare destination address
	sockaddr_in dest_addr;
	memset(&dest_addr, 0, sizeof(dest_addr));
	dest_addr.sin_family = AF_INET;

	if (inet_aton(argv[1], &dest_addr.sin_addr) == 0)
	{
		std::cerr << "Invalid destination address!\n";
		exit(-__COUNTER__);
	}

	// Create the socket
	int sock_fd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);

	if (sock_fd < 0)
	{
		std::cerr << "Unable to create socket!\n";
		exit(-__COUNTER__);
	}


	// 
	auto pid = getpid();
	uint16_t pid_hash = (uint16_t)(2137 * pid + pid);

	icmphdr* packet = new icmphdr;
	bzero(packet, sizeof(icmphdr));

	// True once we encounter ICMP_ECHOREPLY type 
	bool reached = false;

	for(uint8_t c_ttl = 1; c_ttl <= MAX_TTL && !reached; c_ttl++)
	{
		// Set the TTL
		if(setsockopt(sock_fd, IPPROTO_IP, IP_TTL, &c_ttl, sizeof(c_ttl)) )
		{
			std::cerr << "setsockopt(TTL) error!\n";
			exit(-__COUNTER__);
		}

		std::cout << (int)c_ttl<<".\t";

		// Send the packets
		for (int i = 1; i <= BURST_SIZE; ++i)
		{
			packet->type = ICMP_ECHO;
			packet->un.echo.id = pid_hash;
			packet->un.echo.sequence = (uint16_t)c_ttl;
			packet->checksum = 0;

			packet->checksum = checksum((uint16_t*)packet, sizeof(icmphdr));

			if(sendto(sock_fd, packet, sizeof(*packet), 0, (sockaddr*)&dest_addr, sizeof(dest_addr) ) < 1)
			{
				std::cerr << "sendto error!\n";
				exit(-__COUNTER__);
			}
		}

		// Try to receive the responses
		auto resp = receive(sock_fd, pid_hash, c_ttl);
		reached = resp.type == 0 && resp.count > 0;

		if(resp.ips.empty())
		{
			std::cout << "*\n";
			continue;
		}

		for(auto &&ip : resp.ips)
			std::cout << std::setw(16) << ip << " ";

		std::cout << "\t";
	
		if (resp.count != BURST_SIZE)
		{
			std::cout << "???\n";
			continue;
		}

		std::cout << ((double)resp.us)/1e3 << "ms\n";
	}

	delete packet;
	return 0;
}