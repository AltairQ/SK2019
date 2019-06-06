#include "checksum.h"

// Standard ICMP checksum code
uint16_t checksum(uint16_t *buffer, uint32_t size)
{
	unsigned long ck = 0;
	while(size > 1)
	{
		ck += *buffer++;
		size -= (uint32_t)sizeof(uint16_t);
	}

	if(size) ck += *(uint8_t*)buffer;
	
	ck = (ck >> 16) + (ck & 0xffff);
	ck += (ck >> 16);
	return (uint16_t)(~ck);
}
