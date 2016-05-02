#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "transcode.h"

bool GBKToUTF8(const char *gbk, char *utf8, int utf8_size) { 
	int      gbk_size;
	int      utf8_min_size;
	wchar_t  *wc_buffer;

	if (!gbk || !utf8)
		return false;
	else {
		gbk_size = MultiByteToWideChar(CP_ACP, 0, gbk, -1, NULL, 0);
		wc_buffer = (wchar_t*)malloc((gbk_size+1)*sizeof(wchar_t));
		wc_buffer[gbk_size] = 0L;
		MultiByteToWideChar(CP_ACP, 0, gbk, -1, wc_buffer, gbk_size); 
		utf8_min_size = WideCharToMultiByte(CP_UTF8, 0, wc_buffer, -1, NULL, 0, NULL, NULL);
	}

	if (utf8_size < utf8_min_size) {
		free(wc_buffer);
		return false;
	}
	else {
		WideCharToMultiByte(CP_UTF8, 0, wc_buffer, -1, utf8, utf8_min_size, NULL, NULL);
		free(wc_buffer);
		return true;
	}
}

bool UTF8ToGBK(const char *utf8, char *gbk, int gbk_size) { 
	int      utf8_size;
	int      gbk_min_size;
	wchar_t  *wc_buffer;

	if (!utf8 || !gbk)
		return false;
	else {
		utf8_size = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
		wc_buffer = (wchar_t*)malloc((utf8_size+1)*sizeof(wchar_t));
		wc_buffer[utf8_size] = 0L;
		MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wc_buffer, utf8_size); 
		gbk_min_size = WideCharToMultiByte(CP_ACP, 0, wc_buffer, -1, NULL, 0, NULL, NULL);
	}

	if (gbk_size < gbk_min_size) {
		free(wc_buffer);
		return false;
	}
	else {
		WideCharToMultiByte(CP_ACP, 0, wc_buffer, -1, gbk, gbk_min_size, NULL, NULL);
		free(wc_buffer);
		return true;
	}
}