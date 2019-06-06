#include <set>
#include <string>

// Type of value returned by receive(...)
struct ping_response
{
	// Set of IP addresses
	std::set <std::string> ips;

	// Packet count
	size_t count;

	// Mean ping (microseconds)
	long long us;

	// Response packet type
	int type;

	ping_response(): type(-1)
	{

	}
};


