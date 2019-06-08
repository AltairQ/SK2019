#ifndef RESPONSE_H
#define RESPONSE_H

enum HTTP_CODES
{
	HC_200,
	HC_301,
	HC_403,
	HC_404,
	HC_501
};

// static const char* codes_lut[5];
size_t render_hdr_hc200(const char* mime, long int clen);
void send_response_path(int sockfd, const char* fpath);
void send_404(int sockfd);
void send_403(int sockfd);
void send_301(int sockfd, const char* where);
void send_501(int sockfd);

#endif
