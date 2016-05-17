#include "curl_package.h"
#include <stdio.h>
#include <string.h>
#include "curl.h"

#define ASSERT(b) NULL

#pragma comment(lib, "libcurl")
 
static size_t read_callback(void *ptr, size_t size, size_t nmemb, void *stream) {
	size_t retcode = fread(ptr, size, nmemb, (FILE *)stream);
	return retcode;
}

static bool GetFileSizes(const char *p_path, long *p_size) {
	FILE *pf = NULL;
	long size;

	ASSERT(NULL != p_path);
	pf = fopen(p_path, "rb");
	if (NULL == pf) {
		printf("fopen() error! path=%s\n", p_path);
		return false;
	}
	else {
		fseek(pf, 0, SEEK_END);
		size = ftell(pf);
		fseek(pf, 0, SEEK_SET);
		fclose(pf);
		*p_size = size;
		return true;
	}
}

bool ftp_upload(const char *p_local_path, const char *p_remote_path) {
	FILE *pf_local = NULL;	
	CURL *p_handle = NULL;
	CURLcode res;
	curl_off_t file_size;	
	bool result;

	ASSERT(NULL != p_local_path && NULL != p_remote_path);
	pf_local = fopen(p_local_path, "rb");
	if (NULL == pf_local) {
		printf("fopen() error! path=%s\n", p_local_path);
		result = false;
	}
	else {
		curl_global_init(CURL_GLOBAL_ALL); 
		p_handle = curl_easy_init();
		if (NULL == p_handle) {
			printf("curl_easy_init() error!\n");
			result = false;
		}
		else {
			GetFileSizes(p_local_path, (long *)&file_size);
			curl_easy_setopt(p_handle, CURLOPT_READFUNCTION, read_callback);
			curl_easy_setopt(p_handle, CURLOPT_UPLOAD, 1L);
			curl_easy_setopt(p_handle, CURLOPT_URL, p_remote_path);
			curl_easy_setopt(p_handle, CURLOPT_READDATA, pf_local);
			curl_easy_setopt(p_handle, CURLOPT_MAXFILESIZE, file_size);
			curl_easy_setopt(p_handle, CURLOPT_VERBOSE, 0L);	 
			res = curl_easy_perform(p_handle);
			if (res != CURLE_OK) {
				printf("curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
				result = false;
			}
			else {
				result = true;
			}
			curl_easy_cleanup(p_handle);
		}
		curl_global_cleanup();
		fclose(pf_local); 
	}
	return result;
}

 
typedef struct {
	const char *m_path;
	FILE *m_stream;
} t_File;
 
static size_t write_callback(void *buffer, size_t size, size_t nmemb, void *stream) {
	t_File *out = (t_File *)stream;
	size_t write_ret;

	if (NULL == buffer || NULL == stream || NULL == out->m_path) {
		return -1;
	}
	else {
		if (NULL == out->m_stream) {
			out->m_stream = fopen(out->m_path, "wb");
			if (NULL == out->m_stream) {
				return -1;
			}
		}
		write_ret = fwrite(buffer, size, nmemb, out->m_stream);
		return write_ret;
	}
}
 
bool ftp_download(const char *p_remote_path, const char *p_local_path) {
	t_File local_file;
	CURL *p_handle = NULL;
	CURLcode res;
	bool result;
 
	ASSERT(NULL != p_remote_path && NULL != p_local_path);
	curl_global_init(CURL_GLOBAL_DEFAULT);
	p_handle = curl_easy_init();
	if (NULL == p_handle) {
		printf("curl_easy_init() error!\n");
		result = false;
	}
	else {
		local_file.m_path = p_local_path;
		local_file.m_stream = NULL;
		curl_easy_setopt(p_handle, CURLOPT_URL, p_remote_path);
		curl_easy_setopt(p_handle, CURLOPT_WRITEFUNCTION, write_callback);
		curl_easy_setopt(p_handle, CURLOPT_WRITEDATA, &local_file);
		curl_easy_setopt(p_handle, CURLOPT_VERBOSE, 0L);
		res = curl_easy_perform(p_handle);
		if (res != CURLE_OK) {
			printf("curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
			result = false;
		}
		else {
			result = true;
		}
		curl_easy_cleanup(p_handle);
	}
	if (NULL != local_file.m_stream)
		fclose(local_file.m_stream);
	curl_global_cleanup();
	
	return result;
}


#if 1
int main()
{
	char local_path[] = "E:\\5_Photo\\images\\11.jpg";
	char remote_path[] = "ftp://ssyang:ssyang@192.168.1.66/__12345abcde.jpg";
	ftp_upload(local_path, remote_path);

	char local_path2[] = "E:\\abcdefg.txt";
	char remote_path2[] = "ftp://ssyang:ssyang@192.168.1.66/123.txt";
	ftp_download(remote_path2, local_path2);

	return 0;
}
#endif