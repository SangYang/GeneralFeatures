#ifndef _TRANSCODE_H
#define _TRANSCODE_H


bool GBKToUTF8(const char *gbk, char *utf8, int utf8_size);
bool UTF8ToGBK(const char *utf8, char *gbk, int gbk_size);
bool WchToCh(const wchar_t *p_wcstr, char *p_cstr, int c_size);
bool ChToWch(const char *p_cstr, wchar_t *p_wcstr, int wc_size);


#endif // _TRANSCODE_H