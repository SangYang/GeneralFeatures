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
#define DELETE_MAX (1024*1024*10)
#define DEFAULT_CANFPATH "config.ini"

typedef enum {
	LOGTIME_STANDARD = 0,
	LOGTIME_YEAR,
	LOGTIME_MONTH,
	LOGTIME_DAY,
	LOGTIME_HOUR,
	LOGTIME_MINUTE,
	LOGTIME_SECOND,
	LOGTIME_INFINITY
} e_LogTime;

typedef enum {
	LOGCONFIG_DIR = 30,
	LOGCONFIG_SIZE,
	LOGCONFIG_TIME,
	LOGCONFIG_BFILE,
	LOGCONFIG_BLINE,
	LOGCONFIG_BFUNC,
	LOGCONFIG_BPRINT
} e_LogConfig;

typedef struct {
	char m_dir[PATH_MAX];
	int  m_size;
	int  m_time;
	int  m_bfile;
	int  m_bline;
	int  m_bfunc;
	int  m_bprint;
} t_LogConfig;

typedef struct {
	char        m_confpath[PATH_MAX];
	t_LogConfig m_logconfig;
} t_LogPara;

static t_LogPara g_logpara = {DEFAULT_CANFPATH, {"log", 1024*1024*50, LOGTIME_INFINITY, 1, 1, 1, 1}};

const char* Log_GetFileName(const char *path) {
	const char *p_find = NULL;
#ifdef _WIN32
	p_find = strrchr(path, '\\');
#else
	p_find = strrchr(path, '/');
#endif // _WIN32
	if (NULL == p_find)
		return path;
	else
		return (p_find+1);
}

const char* LBfi(const char *file) {
	static char empty[8] = "";
	if (0 >= g_logpara.m_logconfig.m_bfile) 
		return empty;
	else 
		return Log_GetFileName(file);
}

const int LBli(const int line) {
	if (0 >= g_logpara.m_logconfig.m_bline) 
		return 0;
	else
		return line;
}

const char* LBfu(const char *func) {
	static char empty[8] = "";
	if (0 >= g_logpara.m_logconfig.m_bfunc) 
		return empty;
	else 
		return func;
}

#ifdef __linux
static void Log_GetPathFromPid(pid_t pid, char *path, int size) {
    FILE *pf = NULL;
	char *p_find = NULL;	
    char cmd[32];
	
    sprintf(cmd, "readlink /proc/%d/exe", pid);
	pf = popen(cmd, "r");
	if (NULL != pf) {
		fgets(path, size, pf);
		p_find = strrchr(path, '\n');
		if (NULL != p_find)
			*p_find = 0;
		pclose(pf);		
	}
}
#endif // __linux

static void Log_GetModuleName(char *name, int size) {
	char path[PATH_MAX];
#ifdef _WIN32
	GetModuleFileName(NULL, path, sizeof(path));
#else
	pid_t pid = getpid();
	Log_GetPathFromPid(pid, path, sizeof(path));
#endif // _WIN32
	strcpy(name, Log_GetFileName(path));
}

static void Log_GetNowTime(char *nowtime, int size, e_LogTime elogtime) {
	time_t rawtime;
	struct tm *ptmtime;

	time(&rawtime);
	ptmtime = localtime(&rawtime);
	switch (elogtime) {
		case LOGTIME_STANDARD: strftime(nowtime, size, "%y/%m/%d %H:%M:%S", ptmtime); break;
		case LOGTIME_YEAR: strftime(nowtime, size, "%Y", ptmtime); break;
		case LOGTIME_MONTH: strftime(nowtime, size, "%Y%m", ptmtime); break;
		case LOGTIME_DAY: strftime(nowtime, size, "%Y%m%d", ptmtime); break;
		case LOGTIME_HOUR: strftime(nowtime, size, "%Y%m%d%H", ptmtime); break;
		case LOGTIME_MINUTE: strftime(nowtime, size, "%Y%m%d%H%M", ptmtime); break;
		case LOGTIME_SECOND: strftime(nowtime, size, "%Y%m%d%H%M%S", ptmtime); break;
		default: strcpy(nowtime, "Infinity"); break;
	}
}

static void Log_GetLogName(char *logname, e_Log elog) {
	char name[NAME_MAX];
	char time[256];

	Log_GetModuleName(name, sizeof(name));
	Log_GetNowTime(time, sizeof(time), g_logpara.m_logconfig.m_time);
	switch (elog) {
		case LOG_DEBUG: sprintf(logname, "%s.%s.Debug.log", name, time); break;
		case LOG_INFO: sprintf(logname, "%s.%s.Info.log", name, time); break;
		case LOG_WARN: sprintf(logname, "%s.%s.Warn.log", name, time); break;
		case LOG_ERROR: sprintf(logname, "%s.%s.Error.log", name, time); break;
		case LOG_FATAL: sprintf(logname, "%s.%s.Fatal.log", name, time); break;
		default: strcpy(logname, ""); break;
	}
}

static void Log_GetLogPath(char *logpath, e_Log elog) {
	char name[NAME_MAX];
	Log_GetLogName(name, elog);
#ifdef _WIN32
	sprintf(logpath, "%s\\%s", g_logpara.m_logconfig.m_dir, name);
#else
	sprintf(logpath, "%s/%s", g_logpara.m_logconfig.m_dir, name);	
#endif // _WIN32
}

static int Log_GetFileSize(const char *path) {
	struct stat st;
	int stret;

#ifdef _WIN32
	stret = _stat(path, &st);
#else
	stret = stat(path, &st);
#endif // _WIN32
	if (0 == stret) 
		return st.st_size;
	else
		return -1;
}

static void Log_ReadFile(const char *path, char *buffer, int size) {
	FILE *pf = NULL;
	int fsize;

	fsize = Log_GetFileSize(path);
	if (-1 != fsize && fsize < size) {
		pf = fopen(path, "rb");
		if (NULL != pf) {
			fread(buffer, sizeof(char), fsize, pf);
			buffer[fsize] = 0;
			fclose(pf);
		}
	}
}

static void Log_WriteFile(const char *path, const char *buffer, int size) {
	FILE *pf = NULL;

	pf = fopen(path, "wb");
	if (NULL != pf) {
		fwrite(buffer, sizeof(char), size, pf);
		fclose(pf);
	}
}

static void Log_CheckDirectory(const char *dir) {
	char dirtmp[PATH_MAX];
	char tok[PATH_MAX] = "";
	char *p_tok = NULL;
#ifdef _WIN32
	char chset[] = "\\";
#else
	char chset[] = "/";
#endif

#ifdef _WIN32
	if (-1 == access(dir, 0)) {
#else
	if (-1 == access(dir, F_OK)) {		
#endif
		strcpy(dirtmp, dir);
		p_tok = strtok(dirtmp, chset);
		while (NULL != p_tok) {
			strcat(tok, p_tok);
			strcat(tok, chset);
#ifdef _WIN32
			if (-1 == access(tok, 0)) 
				CreateDirectory(tok, NULL);
#else
			if (-1 == access(tok, F_OK))
				mkdir(tok, 0755);		
#endif
			p_tok = strtok(NULL, chset);
		}	
	}
}

static void Log_CheckSize(const char *logpath, e_Log elog) {
	char *buffer = NULL;
	int actsize;
	int delsize;

	if (-1 != access(logpath, 0)) {
		actsize = Log_GetFileSize(logpath);
		if (g_logpara.m_logconfig.m_size < actsize) {
			buffer = (char *)malloc(actsize+1);
			Log_ReadFile(logpath, buffer, actsize+1);
			delsize = (actsize/2 > DELETE_MAX) ? DELETE_MAX : actsize/2;
			Log_WriteFile(logpath, buffer + delsize, (actsize-delsize));
			free(buffer);
		}
	}
}

static int Log_IsPrint(void) {
	static int bFirst = 0;

	if (-1 == access(g_logpara.m_confpath, 0)) 
		return -1;
	else {
		if (0 == bFirst) {
			bFirst = 1;
			Log_Init(g_logpara.m_confpath);
		}
		if (0 >= g_logpara.m_logconfig.m_bprint)
			return -2;
		else {
			return 1;
		}
	}
}

int Log_Write(e_Log elog, const char *fmt, ...) {
	va_list vlist;	
	FILE *pf = NULL;
	char logpath[PATH_MAX];
	char nowtime[TIME_MAX];
	int isret;
	int result = -1;

	isret = Log_IsPrint();
	if (0 < isret) {
		Log_CheckDirectory(g_logpara.m_logconfig.m_dir);
		Log_GetLogPath(logpath, elog);
		Log_CheckSize(logpath, elog);
		pf = fopen(logpath, "ab");
		if (NULL != pf) {
			va_start(vlist, fmt);
			Log_GetNowTime(nowtime, sizeof(nowtime), LOGTIME_STANDARD);
			fprintf(pf, "\r\n%s ", nowtime);
			vfprintf(pf, fmt, vlist);
			va_end(vlist);
			fclose(pf);	
			result = 1;
		}
	}
	return result;	
}

static void Log_ModValue(char *item) {
	char *p_find = NULL;

	p_find = strchr(item, '\r');
	if (NULL != p_find) {
		*p_find = 0;
	}
	p_find = strchr(item, '\n');
	if (NULL != p_find) {
		*p_find = 0;
	}
}

static int Log_ParseConfig(const char *confbuff, char *value, int size, e_LogConfig elogconfig) {
	char *find = NULL;
	char *head = NULL;
	char *tail = NULL;
	char key[ITEM_MAX];
	int keylen;

	switch (elogconfig) {
		case LOGCONFIG_DIR: strcpy(key, "dir="); break;
		case LOGCONFIG_SIZE: strcpy(key, "size="); break;
		case LOGCONFIG_TIME: strcpy(key, "time="); break;
		case LOGCONFIG_BFILE: strcpy(key, "bfile="); break;
		case LOGCONFIG_BLINE: strcpy(key, "bline="); break;
		case LOGCONFIG_BFUNC: strcpy(key, "bfunc="); break;
		case LOGCONFIG_BPRINT: strcpy(key, "bprint="); break;
		default: break;
	}
	keylen = strlen(key);
	find = strstr(confbuff, "[log]");
	if (NULL != find) {
		head = strstr(find, key);
		if (NULL != head) {
			tail = strchr(head, '\n');
			if (NULL != tail && (tail-head-keylen) < size) {
				strncpy(value, head+keylen, tail-head-keylen);
				value[tail-head-keylen] = 0;
				Log_ModValue(value);
				return 1;
			}
		}
	}
	return -1;
}

int Log_Init(const char *confpath) {
	char *confbuff = NULL;
	char dir[ITEM_MAX], size[ITEM_MAX], time[ITEM_MAX];
	char file[ITEM_MAX], line[ITEM_MAX], func[ITEM_MAX], print[ITEM_MAX];
	int dirret, sizeret, timeret, fileret, lineret, funcret, printret;
	int confsize;
	int result = -1;
	
	if (NULL != confpath && -1 != access(confpath, 0)) {
		strcpy(g_logpara.m_confpath, confpath);
		confsize = Log_GetFileSize(confpath);
		if (0 < confsize) {
			confbuff = (char *)malloc(confsize+1);
			Log_ReadFile(confpath, confbuff, confsize+1);
			dirret = Log_ParseConfig(confbuff, dir, sizeof(dir), LOGCONFIG_DIR);
			sizeret = Log_ParseConfig(confbuff, size, sizeof(size), LOGCONFIG_SIZE);
			timeret = Log_ParseConfig(confbuff, time, sizeof(time), LOGCONFIG_TIME);
			fileret = Log_ParseConfig(confbuff, file, sizeof(file), LOGCONFIG_BFILE);
			lineret = Log_ParseConfig(confbuff, line, sizeof(line), LOGCONFIG_BLINE);
			funcret = Log_ParseConfig(confbuff, func, sizeof(func), LOGCONFIG_BFUNC);
			printret = Log_ParseConfig(confbuff, print, sizeof(print), LOGCONFIG_BPRINT);
			if (-1 != dirret) strcpy(g_logpara.m_logconfig.m_dir, dir);
			if (-1 != sizeret) g_logpara.m_logconfig.m_size = atoi(size);
			if (-1 != timeret) {
				if (0 == strcmp(time, "year")) g_logpara.m_logconfig.m_time = LOGTIME_YEAR;
				else if (0 == strcmp(time, "month")) g_logpara.m_logconfig.m_time = LOGTIME_MONTH;
				else if (0 == strcmp(time, "day")) g_logpara.m_logconfig.m_time = LOGTIME_DAY;
				else if (0 == strcmp(time, "hour")) g_logpara.m_logconfig.m_time = LOGTIME_HOUR;
				else if (0 == strcmp(time, "minute")) g_logpara.m_logconfig.m_time = LOGTIME_MINUTE;
				else if (0 == strcmp(time, "second")) g_logpara.m_logconfig.m_time = LOGTIME_SECOND;
				else g_logpara.m_logconfig.m_time = LOGTIME_INFINITY;
			}
			if (-1 != fileret) g_logpara.m_logconfig.m_bfile = atoi(file);
			if (-1 != lineret) g_logpara.m_logconfig.m_bline = atoi(line);
			if (-1 != funcret) g_logpara.m_logconfig.m_bfunc = atoi(func);
			if (-1 != printret) g_logpara.m_logconfig.m_bprint = atoi(print);			
			result = 1;
			free(confbuff);
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

	Log_GetModuleName(name, sizeof(name));
	printf("[main]%s\n", name);
	Log_GetNowTime(time, sizeof(time), LOGTIME_STANDARD);
	printf("[main]%s\n", time);
	Log_GetNowTime(time, sizeof(time), LOGTIME_SECOND);
	printf("[main]%s\n", time);
	Log_GetLogName(logname, LOG_DEBUG);
	printf("[main]%s\n", logname);

	//Log_Init("configaaa.ini");
	printf("[main]%s\n", g_logpara.m_confpath);
	printf("[main]%d\n", g_logpara.m_logconfig.m_time);
	printf("[main]%s\n", g_logpara.m_logconfig.m_dir);
	printf("[main]%d\n", g_logpara.m_logconfig.m_size);
	printf("[main]%d\n", g_logpara.m_logconfig.m_bprint);
	for (i = 0; i < 100000000000000; i++) {
		LOGDEBUG("%s", "abcde");
		LOGINFO("%s", "abcde");
		LOGWARN("%s", "abcde");
		LOGERROR("%s", "abcde");
		LOGFATAL("%s", "abcde");
	}

	return 0;
}
#endif

