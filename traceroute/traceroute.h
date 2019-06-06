#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <errno.h>
#include <cstring>

#define BURST_SIZE 3
#define MAX_TTL 30
