#include "common_log.h"
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif // _WIN32

#define PATH_MAX 256
#define NAME_MAX 128
#define TIME_MAX 64
#define ITEM_MAX 32
#define CMD_MAX  64
#define LOGDEL_MAX (1024 * 1024 * 10)
#define LOG_STYLE_NUM 5
#define LOGCONFIG_PATH "config.ini"

typedef enum {
	LOGCONFIG_ITEM_DIR = 30,
	LOGCONFIG_ITEM_SIZE,
	LOGCONFIG_ITEM_FLAG
} e_LogconfItem;

typedef struct {
	char m_dir[PATH_MAX];
	int  m_size;
	int  m_flag;
} t_LogConfig;

typedef struct {
	char        m_path[PATH_MAX];
	char        m_updatetime[TIME_MAX];
	t_LogConfig m_logconfig;
} t_LogPara;

static t_LogPara g_logpara = {LOGCONFIG_PATH, "2016", {".", 10240, 1}};

#ifdef __linux
static void Log_GetPathFromPid(pid_t pid, char *p_path, const int size) {
    FILE *pf;
	char *p_find;	
    char cmd[CMD_MAX];
	
    sprintf(cmd, "readlink /proc/%d/exe", pid);
	pf = popen(cmd, "r");
	if (NULL != pf) {
		fgets(p_path, size, pf);
		p_find = strrchr(p_path, '\n');
		if (NULL != p_find)
			*p_find = 0;
		pclose(pf);		
	}
}
#endif // __linux

static void Log_GetModuleName(char *p_name, const int size) {
	char path[PATH_MAX];

#ifdef _WIN32
	GetModuleFileName(NULL, path, sizeof(path));
#else
	pid_t pid = getpid();
	Log_GetPathFromPid(pid, path, sizeof(path));
#endif // _WIN32

	strcpy(p_name, Log_GetFileName(path));
}

static void Log_GetNowTime(char *p_nowtime, const int size, const e_LogtimeStyle logtime_style) {
	time_t rawtime;
	struct tm *p_infotime;

	time(&rawtime);
	p_infotime = localtime(&rawtime);
	switch (logtime_style) {
		case LOGTIME_STYLE_STANDARD: strftime(p_nowtime, size, "%Y-%m-%d %H:%M:%S", p_infotime); break;
		case LOGTIME_STYLE_SIMPLE: strftime(p_nowtime, size, "%y/%m/%d %H:%M:%S", p_infotime); break;
		case LOGTIME_STYLE_YEAR: strftime(p_nowtime, size, "%Y", p_infotime); break;
		case LOGTIME_STYLE_MONTH: strftime(p_nowtime, size, "%Y%m", p_infotime); break;
		case LOGTIME_STYLE_DAY: strftime(p_nowtime, size, "%Y%m%d", p_infotime); break;
		case LOGTIME_STYLE_HOUR: strftime(p_nowtime, size, "%Y%m%d%H", p_infotime); break;
		case LOGTIME_STYLE_MINUTE: strftime(p_nowtime, size, "%Y%m%d%H%M", p_infotime); break;
		case LOGTIME_STYLE_SECOND: strftime(p_nowtime, size, "%Y%m%d%H%M%S", p_infotime); break;
		default: break;
	}
}

static void Log_GetLogName(char *p_logname, const e_LogStyle logstyle) {
	char name[NAME_MAX];
	char time[256];

	Log_GetModuleName(name, sizeof(name));
	strcpy(time, g_logpara.m_updatetime);
	switch (logstyle) {
		case LOG_STYLE_DEBUG: sprintf(p_logname, "%s.%s.Debug.log", name, time); break;
		case LOG_STYLE_INFO: sprintf(p_logname, "%s.%s.Info.log", name, time); break;
		case LOG_STYLE_WARN: sprintf(p_logname, "%s.%s.Warn.log", name, time); break;
		case LOG_STYLE_ERROR: sprintf(p_logname, "%s.%s.Error.log", name, time); break;
		case LOG_STYLE_FATAL: sprintf(p_logname, "%s.%s.Fatal.log", name, time); break;
		default: break;
	}
}

static void Log_GetLogPath(char *p_logpath, const e_LogStyle logstyle) {
	char name[NAME_MAX];

	Log_GetLogName(name, logstyle);
#ifdef _WIN32
	sprintf(p_logpath, "%s\\%s", g_logpara.m_logconfig.m_dir, name);
#else
	sprintf(p_logpath, "%s/%s", g_logpara.m_logconfig.m_dir, name);	
#endif // _WIN32
}

static void Log_ModValue(char *p_item) {
	char *p_find = NULL;

	p_find = strchr(p_item, '\r');
	if (NULL != p_find) {
		*p_find = 0;
	}
	p_find = strchr(p_item, '\n');
	if (NULL != p_find) {
		*p_find = 0;
	}
}

static int Log_ParseConfig(const char *p_confbuff, char *p_item, e_LogconfItem logconfitem) {
	char *p_find = NULL;
	char *p_head = NULL;
	char *p_tail = NULL;
	char item[ITEM_MAX];
	int itemlen;

	switch (logconfitem) {
		case LOGCONFIG_ITEM_DIR: strcpy(item, "dir="); break;
		case LOGCONFIG_ITEM_SIZE: strcpy(item, "size="); break;
		case LOGCONFIG_ITEM_FLAG: strcpy(item, "flag="); break;
		default: break;
	}
	itemlen = strlen(item);
	p_find = strstr(p_confbuff, "[log]");
	if (NULL != p_find) {
		p_head = strstr(p_find, item);
		if (NULL != p_head) {
			p_tail = strchr(p_head, '\n');
			if (NULL != p_tail) {
				strncpy(p_item, p_head+itemlen, p_tail-p_head-itemlen);
				p_item[p_tail-p_head-itemlen] = 0;
				Log_ModValue(p_item);
				return 1;
			}
		}
	}
	return -1;
}

static int Log_GetFileSize(const char *p_path) {
	struct stat st;
	int stret;

#ifdef _WIN32
	stret = _stat(p_path, &st);
#else
	stret = stat(p_path, &st);
#endif // _WIN32
	if (0 == stret) 
		return st.st_size;
	return -1;
}

static int Log_GetFileBuffer(const char *p_path, char *p_buffer) {
	FILE *pf = NULL;
	int size;

	size = Log_GetFileSize(p_path);
	if (-1 != size) {
		pf = fopen(p_path, "rb");
		if (NULL != pf) {
			fread(p_buffer, sizeof(char), size, pf);
			p_buffer[size] = 0;
			fclose(pf);
			return 1;
		}
	}
	return -1;
}

static void Log_UpdateFileBuffer(const char *p_path, const char *p_buffer, const int size) {
	FILE *pf = NULL;

	pf = fopen(p_path, "wb");
	if (NULL != pf) {
		fwrite(p_buffer, sizeof(char), size, pf);
		fclose(pf);
	}
}

static void Log_CheckDirectory(const char *p_dir) {
	char dir[PATH_MAX];
	char tok[PATH_MAX] = "";
	char *p_tok = NULL;
#ifdef _WIN32
	char chset[] = "\\";
#else
	char chset[] = "/";
#endif // WIN32

	strcpy(dir, p_dir);
	p_tok = strtok(dir, chset);
	while (NULL != p_tok) {
		strcat(tok, p_tok);
		strcat(tok, chset);
#ifdef _WIN32
		CreateDirectory(tok, NULL);
#else
		if (0 != access(tok, F_OK)) {
			int aa= mkdir(tok, 0755);
			printf("[dir]%d, %s, %d %s\n", aa, tok, errno, strerror(errno));			
		}
#endif // _WIN32
		p_tok = strtok(NULL, chset);
	}
}

static void Log_CheckSize(const char *p_dir, const int size, const e_LogStyle logstyle) {
	char path[PATH_MAX];
	char *p_buffer = NULL;
	int actsize;
	int delsize;

	Log_GetLogPath(path, logstyle);
	actsize = Log_GetFileSize(path);
	if (1024 * size < actsize) {
		p_buffer = (char *)malloc(actsize+1);
		Log_GetFileBuffer(path, p_buffer);
		delsize = (actsize/2 > LOGDEL_MAX) ? LOGDEL_MAX : actsize/2;
		Log_UpdateFileBuffer(path, p_buffer + delsize, (actsize-delsize));
		free(p_buffer);
	}
}

const char* Log_GetFileName(const char *p_path) {
	const char *p_find = NULL;

#ifdef _WIN32
	p_find = strrchr(p_path, '\\');
#else
	p_find = strrchr(p_path, '/');
#endif // _WIN32
	if (NULL == p_find)
		return p_path;
	else
		return (p_find+1);
}

int Log_Init(const char *p_path, e_LogtimeStyle logtime_style) {
	char *p_confbuff = NULL;
	char dir[ITEM_MAX], size[ITEM_MAX], flag[ITEM_MAX];
	int dirret, sizeret, flagret;
	int confsize;
	int result = -1;
	
	if (NULL == p_path)
		confsize = Log_GetFileSize(g_logpara.m_path);
	else {
		confsize = Log_GetFileSize(p_path);
		strcpy(g_logpara.m_path, p_path);
	}
	if (0 >= confsize) 
		result = -2;
	else {
		p_confbuff = (char *)malloc(confsize+1);
		Log_GetFileBuffer(g_logpara.m_path, p_confbuff);
		dirret = Log_ParseConfig(p_confbuff, dir, LOGCONFIG_ITEM_DIR);
		sizeret = Log_ParseConfig(p_confbuff, size, LOGCONFIG_ITEM_SIZE);
		flagret = Log_ParseConfig(p_confbuff, flag, LOGCONFIG_ITEM_FLAG);
		printf("[init]dir=%s,size=%s,flag=%s\n", dir, size, flag);
		if (-1 == dirret || -1 == sizeret || -1 == flagret)
			result = -3;
		else {	
			strcpy(g_logpara.m_logconfig.m_dir, dir);
			g_logpara.m_logconfig.m_size = atoi(size);
			g_logpara.m_logconfig.m_flag = atoi(flag);
			Log_GetNowTime(g_logpara.m_updatetime, TIME_MAX, logtime_style);
			Log_CheckDirectory(g_logpara.m_logconfig.m_dir);
			result = 1;
		}
		free(p_confbuff);
	}

	return result;
}

static int Log_IsPrint(const e_LogStyle logstyle) {
	if (-1 == access(g_logpara.m_path, 0)) 
		return -1;
	else if (1 != g_logpara.m_logconfig.m_flag)
		return -2;
	else {
		Log_CheckSize(g_logpara.m_logconfig.m_dir, g_logpara.m_logconfig.m_size, logstyle);
		return 1;
	}
}

int Log_Write(const e_LogStyle logstyle, const char *p_format, ...) {
	va_list vlist;	
	FILE *pf = NULL;
	char path[PATH_MAX];
	char nowtime[TIME_MAX];
	int isret;
	int result;

	isret = Log_IsPrint(logstyle);
	if (1 != isret)
		result = -1;
	else {
		Log_GetLogPath(path, logstyle);
		pf = fopen(path, "ab");
		if (NULL != pf) {
			va_start(vlist, p_format);
#ifdef USE_LOGTIME_STYLE_STANDARD
			Log_GetNowTime(nowtime, sizeof(nowtime), LOGTIME_STYLE_STANDARD);
#elif defined(USE_LOGTIME_STYLE_SECOND)
			Log_GetNowTime(nowtime, sizeof(nowtime), LOGTIME_STYLE_SECOND);
#elif defined(USE_LOGTIME_STYLE_SIMPLE)
			Log_GetNowTime(nowtime, sizeof(nowtime), LOGTIME_STYLE_SIMPLE);
#else
			Log_GetNowTime(nowtime, sizeof(nowtime), LOGTIME_STYLE_STANDARD);
#endif // USE_LOGTIME_STYLE_*
			fprintf(pf, "\r\n%s ", nowtime);
			result = vfprintf(pf, p_format, vlist);
			va_end(vlist);
			fclose(pf);	
		}
	}
	return result;	
}

#if 1
int main(void) {
	char name[NAME_MAX];
	char time[TIME_MAX];
	char logname[NAME_MAX];
	int printret;
	int i;

	printf("[main]start\n");
	Log_GetModuleName(name, sizeof(name));
	printf("[main]%s\n", name);

	Log_GetNowTime(time, sizeof(time), LOGTIME_STYLE_STANDARD);
	printf("[main]%s\n", time);
	Log_GetNowTime(time, sizeof(time), LOGTIME_STYLE_SECOND);
	printf("[main]%s\n", time);

	Log_GetLogName(logname, LOG_STYLE_DEBUG);
	printf("[main]%s\n", logname);

	Log_Init(NULL, LOGTIME_STYLE_HOUR);
	printf("[main]%s\n", g_logpara.m_path);
	printf("[main]%s\n", g_logpara.m_updatetime);
	printf("[main]%s\n", g_logpara.m_logconfig.m_dir);
	printf("[main]%d\n", g_logpara.m_logconfig.m_size);
	printf("[main]%d\n", g_logpara.m_logconfig.m_flag);
	for (i = 0; i < 100000000000000; i++) {
/*
		Log_Write(LOG_STYLE_DEBUG, "%s", "abcde");
		Log_Write(LOG_STYLE_INFO, "%s", "abcde");
		Log_Write(LOG_STYLE_WARN, "%s", "abcde");
		Log_Write(LOG_STYLE_ERROR, "%s", "abcde");
		Log_Write(LOG_STYLE_FATAL, "%s", "abcde");*/
		LOGDEBUG("%s", "abcde");
		LOGINFO("%s", "abcde");
		LOGWARN("%s", "abcde");
		LOGERROR("%s", "abcde");
		LOGFATAL("%s", "abcde");
	}


	return 0;
}
#endif
