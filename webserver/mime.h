#ifndef MIME_H
#define MIME_H

enum MIME
{
	TXT,
	HTML,
	CSS,
	JPG,
	JPEG,
	PNG,
	PDF,
	OTHER
};

enum MIME path_to_mime(const char* path);
const char* path_to_mime_name(const char* path);
const char* mimes_lut[8];

#endif
