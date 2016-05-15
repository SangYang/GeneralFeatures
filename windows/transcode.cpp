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

