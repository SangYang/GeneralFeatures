#ifndef _TRANSCODE_H
#define _TRANSCODE_H


bool GBKToUTF8(const char *gbk, char *utf8, int utf8_size);
bool UTF8ToGBK(const char *utf8, char *gbk, int gbk_size);


#endif // _TRANSCODE_H