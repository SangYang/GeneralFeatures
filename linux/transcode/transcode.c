#include "transcode.h"
#include <errno.h>
#include <iconv.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool IconvCode(const char *fromcode, const char *tocode, const char *inbuf, char *outbuf, int *outsize) {
	iconv_t cd;
	int iret;
	int inlen;
	int outlen;
	bool okres;
	
	cd = iconv_open(tocode, fromcode);
	if (-1 != (int)cd) {
		inlen = strlen(inbuf) + 1;
		outlen = *outsize;
		iret = iconv(cd, (char **)&inbuf, &inlen, &outbuf, &outlen);		
		if (-1 != iret && 0 == inlen) {			
			*outsize -= outlen;	
			okres = true;
		}
		else {
			printf("[error]iconv(%d,%s,%d,%s,%d)=%d, {%d:%s}\n", 
				(int)cd, inbuf, inlen, outbuf, outlen, iret, errno, strerror(errno));
			okres = false;
		}
		iconv_close(cd);		
	}
	else {
		printf("[error]iconv_open(%s,%s)=%d, {%d:%s}\n", tocode, fromcode, (int)cd, errno, strerror(errno));	
		okres = false;
	}
	return okres;
}

// http://192.168.1.96/store/2016051806/ITS_上海路-0-晋A0004-20160517185016004_II_OL_OV%20(430).JPG
// http://192.168.1.96/store/2016051806/ITS_%E4%B8%8A%E6%B5%B7%E8%B7%AF-0-%E6%99%8BA0004-20160517185016004_II_OL_OV%20(430).JPG
bool UTF8toURL(const char *p_utf8, char *p_url, const int url_size) {
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
		printf("UTF8toURL() error! Input para is NULL!\n");
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
			printf("UTF8toURL() error! Input para url_size is small!\n");
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
int main(int argc, char *argv[]) {
	char gbk[256] = "abcdefga大大的理解123456789";
	char utf8[256] = "http://192.168.1.96/store/2016051806/ITS_上海路-0-晋A0004-20160517185016004_II_OL_OV%20(430).JPG";
	char url[256];
	int uft8size = 25;//sizeof(utf8);
	
	//IconvCode("UTF-8", "GBK", gbk, utf8, &uft8size);
	printf("[gbk]%s\n", gbk);	
	printf("[utf8]%s\n", utf8);
	
	UTF8toURL(utf8, url, sizeof(url));
	printf("[url]%s\n", url);
	
	return 0;
}
#endif

#if 0
#include <wchar.h>
int main() {
	char ch[256] = "123456";
	wchar_t wch[256] = L"abcdefghijklmn";
	int tosize;
	
	//tosize = mbstowcs(wch, ch, strlen(ch)+1);
	tosize = wcstombs(ch, wch, wcslen(wch)+1);
	printf("tosize=%d\n", tosize);	
	printf("ch=%s\n", ch);
	printf("wch=%ls\n", wch);
	
	return 0;
}
#endif

