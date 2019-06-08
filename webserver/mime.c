#include "common.h"
#include "mime.h"


const char* mimes_lut[] = {
	"text/plain",
	"text/html",
	"text/css",
	"image/jpeg",
	"image/jpeg",
	"image/png",
	"application/pdf",
	"application/octet-stream"
};


enum MIME path_to_mime(const char* path)
{
	char* ext = strrchr(path, '.');
	if(ext == NULL)
		return OTHER;

	ext++;

	if (strcmp("txt", ext) == 0)
		return TXT;
	else if (strcmp("html", ext) == 0)
		return HTML;
	else if (strcmp("css", ext) == 0)
		return CSS;
	else if (strcmp("jpg", ext) == 0)
		return JPG;
	else if (strcmp("jpeg", ext) == 0)
		return JPEG;
	else if (strcmp("png", ext) == 0)
		return PNG;
	else if (strcmp("pdf", ext) == 0)
		return PDF;

	return OTHER;
}

const char* path_to_mime_name(const char* path)
{
	return mimes_lut[path_to_mime(path)];
}
