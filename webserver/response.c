#include "common.h"
#include "response.h"
#include "mime.h"

#define HDRBUF_SIZE 2048
#define FILE_CHONK 1024
char hbuf[HDRBUF_SIZE];
char fbuf[FILE_CHONK];

static const char* codes_lut[] =
{
	"HTTP/1.1 200 OK\r\n"
	"Server: magi\r\n"
	"X-XSS-Protection: 0\r\n"
	"X-Frame-Options: SAMEORIGIN\r\n"
	"Content-Type: %s\r\n"
	"Content-Length: %ld\r\n"
	"\r\n",

	"HTTP/1.1 301 Moved Permanently\r\n"
	"Server: magi\r\n"
	"Location: %s\r\n"
	"Connection: keep-alive\r\n"
	"Content-Length: 0\r\n"
	"\r\n"
	,
	
	"HTTP/1.1 403 Forbidden\r\n"
	"Server: magi\r\n"
	"Content-Type: text/html\r\n"
	"Content-Length: 43\r\n"
	"\r\n"
	"<html><body><h1>Go away!</h1></body></html>",

	"HTTP/1.1 404 Not Found\r\n"
	"Server: magi\r\n"
	"Content-Type: text/html\r\n"
	"Content-Length: 60\r\n"
	"\r\n"
	"<html><body><h1>404, nothing to see here.</h1></body></html>",

	"HTTP/1.1 501 Not Implemented\r\n"
	"Server: magi\r\n"
	"Content-Type: text/html\r\n"
	"Content-Length: 60\r\n"
	"\r\n"
	"<html><body><h1>501 wtf.</h1></body></html>"
};


size_t render_hdr_hc200(const char* mime, long int clen)
{
	return (size_t)snprintf(hbuf, HDRBUF_SIZE, codes_lut[HC_200],
		mime, // Content-Type
		clen // Content-Length
		);
}

size_t render_hdr_hc404()
{
	return (size_t)snprintf(hbuf, HDRBUF_SIZE, codes_lut[HC_404]);
}

size_t render_hdr_hc403()
{
	return (size_t)snprintf(hbuf, HDRBUF_SIZE, codes_lut[HC_403]);
}

size_t render_hdr_hc301(const char* dest_path)
{
	return (size_t)snprintf(hbuf, HDRBUF_SIZE, codes_lut[HC_301],
		dest_path
		);
}

size_t render_hdr_hc501()
{
	return (size_t)snprintf(hbuf, HDRBUF_SIZE, codes_lut[HC_501]);
}


void send_response_path(int sockfd, const char* fpath)
{
	int fd = open(fpath, O_RDONLY);

	struct stat tmpstat;
	fstat(fd, &tmpstat);

	if(fd < 0)
		ERROR("Requested file open error");

	if(send(sockfd, hbuf, render_hdr_hc200(
		path_to_mime_name(fpath),
		tmpstat.st_size),
		0) < 0)
		ERROR("send failed");


	ssize_t cchonk;

	while(true)
	{
		if((cchonk = read(fd, fbuf, FILE_CHONK)) < 0)
			ERROR("send_response_path.read error");

		if(send(sockfd, fbuf, (size_t)cchonk, 0) != cchonk)
			ERROR("send_response_path.send error");

		if(cchonk < FILE_CHONK)
			break;
	}

	close(fd);
}

void send_404(int sockfd)
{
	if(send(sockfd, hbuf, render_hdr_hc404(), 0) < 0)
		ERROR("send failed");
}

void send_403(int sockfd)
{
	if(send(sockfd, hbuf, render_hdr_hc403(), 0) < 0)
		ERROR("send failed");
}

void send_301(int sockfd, const char* where)
{
	if(send(sockfd, hbuf, render_hdr_hc301(where), 0) < 0)
		ERROR("send failed");
}

void send_501(int sockfd)
{
	if(send(sockfd, hbuf, render_hdr_hc501(), 0) < 0)
		ERROR("send failed");
}
