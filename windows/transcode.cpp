#include "stdafx.h"
#include <locale.h>
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


bool WchToCh(const wchar_t *p_wcstr, char *p_cstr, int c_size) {
	int c_len;
	int wc_len;	
	int copy_size;

	if (NULL == p_cstr || NULL == p_wcstr) {
		printf("WchToCh() error! Input para is NULL!");
		return false;
	}
	else {
		wc_len = wcslen(p_wcstr);
		c_len = wc_len * 2;
		if (c_size <= c_len) {
			printf("WchToCh() error! c_size too small!\n");
			return false;			
		}
		else {
			copy_size = WideCharToMultiByte(0, 0, p_wcstr, wc_len + 1, p_cstr, c_size, NULL, NULL); 
			if (c_size < copy_size) {
				printf("WideCharToMultiByte() error!\n");
				return false;
			}   
			else {
				return true;	
			}			
		}
	}
}

bool ChToWch(const char *p_cstr, wchar_t *p_wcstr, int wc_size) {
	int c_len;
	int wc_len;	
	int copy_size;

	if (NULL == p_cstr || NULL == p_wcstr) {
		printf("ChToWch() error! Input para is NULL!");
		return false;
	}
	else {
		c_len = strlen(p_cstr);
		wc_len = c_len / 2 + c_len % 2;
		if (wc_size <= wc_len) {
			printf("ChToWch() error! wc_size too small!\n");
			return false;			
		}
		else {
			copy_size = MultiByteToWideChar(0, 0, p_cstr, c_len + 1, p_wcstr, wc_size);
			if (wc_size < copy_size) {
				printf("MultiByteToWideChar() error!\n");
				return false;
			}   
			else {
				return true;	
			}
		}
	}
}

// http://192.168.1.96/store/2016051806/ITS_上海路-0-晋A0004-20160517185016004_II_OL_OV%20(430).JPG
// http://192.168.1.96/store/2016051806/ITS_%E4%B8%8A%E6%B5%B7%E8%B7%AF-0-%E6%99%8BA0004-20160517185016004_II_OL_OV%20(430).JPG
bool UTF8ToURL(const char *p_utf8, char *p_url, const int url_size) {
	const char ch_set[] = 
		"abcdefghijklmnopqrstuvwxyz"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"0123456789"
		"$-_.+!*'(),"
		";/?:@=&";
	const char *p_find = NULL;	
	char *p_temp_url = NULL;
	int index;
	int utf8_len;

	if (NULL == p_utf8 || NULL == p_url) {
		printf("UTF8ToURL() error! Input para is NULL!\n");
		return false;
	}
	else {
		utf8_len = strlen(p_utf8);
		p_temp_url = (char *)malloc(utf8_len * 2);
		memset(p_temp_url, 0, sizeof(utf8_len * 2));
		for (index = 0; index < utf8_len; index++) {
			p_find = strchr(ch_set, p_utf8[index]);
			if (NULL == p_find) {
				sprintf(p_temp_url, "%s%%%02X", p_temp_url, (unsigned char)p_utf8[index]);
			}
			else {
				sprintf(p_temp_url, "%s%c", p_temp_url, p_utf8[index]);
			}
		}
		sprintf(p_temp_url, "%s%c", p_temp_url, 0);
		if (strlen(p_temp_url) >= url_size) {
			printf("UTF8ToURL() error! Input para url_size is small!\n");
			free(p_temp_url);
			return false;
		}
		else {
			memcpy(p_url, p_temp_url, strlen(p_temp_url) + 1);
			free(p_temp_url);
			return true;
		}
	}
}


#if 0
int main(int argc, char* argv[]) {
	wchar_t p_wcstr[] = L"我是中国人";
	wchar_t p_wcstr2[20];
	char *p_cstr = NULL;
	int c_size;

	setlocale(LC_ALL, "chs");  
	c_size = sizeof(p_wcstr) - 1;
	p_cstr = (char *)malloc(c_size + 1);
	WchToCh(p_wcstr, p_cstr, c_size);
	printf("cstr=%s\n", p_cstr);

	ChToWch(p_cstr, p_wcstr2, sizeof(p_wcstr));
	wprintf(L"wcstr=%s\n", p_wcstr);

	free(p_cstr) ;
	return 0;
}
#endif

