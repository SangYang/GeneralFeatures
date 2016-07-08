#ifndef LINUX_TRANSCODE_H
#define LINUX_TRANSCODE_H


#ifndef __cplusplus
#define bool   int
#define false  0
#define true   1
#endif // __cplusplus

#ifdef __cplusplus
extern "C" {
#endif

	bool IconvCode(const char *fromcode, const char *tocode, const char *inbuf, char *outbuf, int *outsize);
	bool UTF8toURL(const char *p_utf8, char *p_url, const int url_size);

#ifdef __cplusplus
};
#endif


#endif // LINUX_TRANSCODE_H

