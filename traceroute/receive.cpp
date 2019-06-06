#include <chrono>

#include "traceroute.h"
#include "ping_response.h"


ping_response receive(int sockfd, uint16_t pid_hash, uint16_t seq)
{
	// This is the return struct
	ping_response resp;

	// Initial timeout is 1sec
	timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 0;

	// Get the start time
	auto start_time = std::chrono::high_resolution_clock::now();

	// Remaining time in microseconds
	long long rem_us = 1e06;

	// Sum of delays so far (in us)
	long long timesum = 0;

	// Current packet count
	int c_packetcnt = 0;

	// Default is ICMP_ECHOREPLY
	resp.type = 0;

	while(rem_us > 0 && c_packetcnt < BURST_SIZE)
	{

		// Prepare address structs
		sockaddr_in 	sender;	
		socklen_t 			sender_len = sizeof(sender);
		u_int8_t 			buffer[IP_MAXPACKET];


		// Set the timeout
		if(setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0)
		{
			std::cerr << "Error setting timeout! " << strerror(errno) << "\n";
			exit(-__COUNTER__);
		}

		// Try to receive a packet
		ssize_t packet_len = recvfrom (sockfd, buffer, IP_MAXPACKET, 0, (sockaddr*)&sender, &sender_len);

		if(packet_len < 0)
		{
			// We ran out of time
			if ( errno == EAGAIN || errno == EWOULDBLOCK)
				break;

			// General error
			fprintf(stderr, "recvfrom error: %s\n", strerror(errno)); 
			exit(-__COUNTER__);
		}

		// Calculate time interval from the start
		auto interval = std::chrono::high_resolution_clock::now() - start_time;
		auto us_interval = std::chrono::duration_cast<std::chrono::microseconds>(interval).count();

		// Remaining time = 1s - (us_interval) * 1us
		rem_us = (long long)1e6 - us_interval;

		// Set the next timeout
		tv.tv_sec = 0;
		tv.tv_usec = rem_us;

		// Parsing the ICMP response
		// Get offsets from iphdr contents
		struct iphdr* 		ip_header = (struct iphdr*) buffer;
		ssize_t				ip_header_len = 4 * ip_header->ihl;


		icmphdr* response = (icmphdr*) (buffer + ip_header_len);

		// If type == 11 we need additional offset to access the original header
		if(response->type == ICMP_TIME_EXCEEDED)
			response = (icmphdr*) (buffer + ip_header_len + ip_header_len + 8);
		
		// If this packet does not belong to the current burst, just ignore
		if(response->un.echo.id != pid_hash || response->un.echo.sequence != seq)
			continue;

		// This is so that a single ECHOREPLY won't stop us from continuing with TTLs
		if(response->type != 0)
			resp.type = response->type;

		// Successfully received our packet
		c_packetcnt++;
		timesum += us_interval;

		// Get IP address string and add to the result
		char sender_ip_str[20]; 
		inet_ntop(AF_INET, &(sender.sin_addr), sender_ip_str, sizeof(sender_ip_str));
		resp.ips.insert(std::string(sender_ip_str));

	}


	resp.count = c_packetcnt;
	resp.us = (resp.count != 0)? timesum / (resp.count) : std::numeric_limits<long long>::max();

	return resp;
		
}