#include "debug_log.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#pragma warning(disable : 4996)

#define INI_FILE_PATH    "test.ini"
#define LOG_FILE_PATH    "test.log"
#define LOGSIZE_MAX      1024 * 1024 * 10

bool HaveIni() {
	FILE *pf_ini;

	pf_ini = fopen(INI_FILE_PATH, "rb");
	if (NULL == pf_ini)
		return false;
	else {
		fclose(pf_ini);
		return true;
	}
}

static bool HaveIniWithSize(int *p_logsize) {
	FILE *pf_ini;
	int inisize;
	char logsize[16];

	pf_ini = fopen(INI_FILE_PATH, "rb");
	if (NULL == pf_ini)
		return false;
	else {
		fseek(pf_ini, 0, SEEK_END);
		inisize = ftell(pf_ini);
		fseek(pf_ini, 0, SEEK_SET);
		if (0 < inisize)
			fread(logsize, sizeof(char), inisize, pf_ini);
		logsize[inisize] = '\0';
		*p_logsize = atoi(logsize);
		fclose(pf_ini);
		return true;
	}
}

const char* GetFileName(const char *p_path) {
	const char *p_find = NULL;

#ifdef _WIN32
	p_find = strrchr(p_path, '\\');
#else
	p_find = strrchr(p_path, '/');
#endif // _WIN32
	if (NULL == p_find)
		return p_path;
	else
		return p_find + 1;
}


static bool GetCurrentTimes(char *p_current_time, const int time_size) {
	const int c_TimeSizeMin = sizeof("20151122101033");
	struct tm *p_tm_time;
	time_t i_time;

	if (NULL == p_current_time || c_TimeSizeMin > time_size)
		return false;
	else {
		time(&i_time);
		p_tm_time = localtime(&i_time);
		if (NULL == p_tm_time)
			return false;
		else {
			sprintf(p_current_time, "%04d%02d%02d%02d%02d%02d", 
				p_tm_time->tm_year + 1900, p_tm_time->tm_mon + 1, p_tm_time->tm_mday,
				p_tm_time->tm_hour, p_tm_time->tm_min, p_tm_time->tm_sec);
			return true;
		}
	}
}

static void CheckLogSize(const char *p_path, const int logsize) {
	FILE *pf = NULL;
	char *p_buffer = NULL;
	int size;
	int sizemax;

	pf = fopen(p_path, "rb"); 
	if (NULL != pf) {
		fseek(pf, 0, SEEK_END);
		size = ftell(pf);
		fseek(pf, 0, SEEK_SET);	
		if (0 < logsize)
			sizemax = logsize;
		else
			sizemax = LOGSIZE_MAX;
		if (sizemax < size) {
			p_buffer = (char *)malloc(size + 1);
			if (NULL != p_buffer) {
				fread(p_buffer, sizeof(char), size, pf);
			}
		}
		fclose(pf);
	}
		
	if (NULL != p_buffer) {
		pf = fopen(p_path, "wb"); 
		if (NULL != pf) {
			fwrite(p_buffer + size - size / 2, sizeof(char), size / 2, pf);
			fclose(pf);
		}		
		free(p_buffer);
	}
}

int WriteLog(const char *p_format, ...) {
	FILE *pf_log;
	bool have_ini;
	char current_time[32];
	va_list arg_ptr;
	int writed_byte;
	int log_size;

	have_ini = HaveIniWithSize(&log_size);
	if (false == have_ini)
		writed_byte = 0;
	else {
		CheckLogSize(LOG_FILE_PATH, log_size);
		pf_log = fopen(LOG_FILE_PATH, "ab");
		if (NULL == pf_log)
			writed_byte = 0;
		else {
			va_start(arg_ptr, p_format);
			GetCurrentTimes(current_time, sizeof(current_time));
			fprintf(pf_log, "\r\n%s  ", current_time);
			writed_byte = vfprintf(pf_log, p_format, arg_ptr);
			va_end(arg_ptr);
			fclose(pf_log);	
		}	
	}
	return writed_byte;
}


