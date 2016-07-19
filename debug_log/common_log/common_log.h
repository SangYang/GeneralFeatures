#ifndef COMMON_LOG_H
#define COMMON_LOG_H


#define USE_LOGSTANDARD

#if defined(_WIN32) && defined(USE_LOGSTANDARD)
#define LOGDEBUG(fmt, ...)  Log_Write(LOG_DEBUG, "[%s|%d|%s] "##fmt, LBfi(__FILE__), LBli(__LINE__), LBfu(__FUNCTION__), ##__VA_ARGS__)
#define LOGINFO(fmt, ...)   Log_Write(LOG_INFO, "[%s|%d|%s] "##fmt, LBfi(__FILE__), LBli(__LINE__), LBfu(__FUNCTION__), ##__VA_ARGS__)
#define LOGWARN(fmt, ...)   Log_Write(LOG_WARN, "[%s|%d|%s] "##fmt, LBfi(__FILE__), LBli(__LINE__), LBfu(__FUNCTION__), ##__VA_ARGS__)
#define LOGERROR(fmt, ...)  Log_Write(LOG_ERROR, "[%s|%d|%s] "##fmt, LBfi(__FILE__), LBli(__LINE__), LBfu(__FUNCTION__), ##__VA_ARGS__)
#define LOGFATAL(fmt, ...)  Log_Write(LOG_FATAL, "[%s|%d|%s] "##fmt, LBfi(__FILE__), LBli(__LINE__), LBfu(__FUNCTION__), ##__VA_ARGS__)
#elif defined(__linux) && defined(USE_LOGSTANDARD)
#define LOGDEBUG(fmt, ...)  Log_Write(LOG_DEBUG, "[%s|%d|%s] "fmt, LBfi(__FILE__), LBli(__LINE__), LBfu(__FUNCTION__), ##__VA_ARGS__)
#define LOGINFO(fmt, ...)   Log_Write(LOG_INFO, "[%s|%d|%s] "fmt, LBfi(__FILE__), LBli(__LINE__), LBfu(__FUNCTION__), ##__VA_ARGS__)
#define LOGWARN(fmt, ...)   Log_Write(LOG_WARN, "[%s|%d|%s] "fmt, LBfi(__FILE__), LBli(__LINE__), LBfu(__FUNCTION__), ##__VA_ARGS__)
#define LOGERROR(fmt, ...)  Log_Write(LOG_ERROR, "[%s|%d|%s] "fmt, LBfi(__FILE__), LBli(__LINE__), LBfu(__FUNCTION__), ##__VA_ARGS__)
#define LOGFATAL(fmt, ...)  Log_Write(LOG_FATAL, "[%s|%d|%s] "fmt, LBfi(__FILE__), LBli(__LINE__), LBfu(__FUNCTION__), ##__VA_ARGS__)
#else
#define LOGDEBUG(fmt, ...)  ((void *)0)
#define LOGINFO(fmt, ...)   ((void *)0)
#define LOGWARN(fmt, ...)   ((void *)0)
#define LOGERROR(fmt, ...)  ((void *)0)
#define LOGFATAL(fmt, ...)  ((void *)0)
#endif

typedef enum {
	LOG_DEBUG = 100,
	LOG_INFO,
	LOG_WARN,
	LOG_ERROR,
	LOG_FATAL
} e_Log;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

	const char* LBfi(const char *file);
	const char* LBfu(const char *func);
	const int LBli(const int line);
	int Log_Init(const char *confpath);
	int Log_Write(e_Log elog, const char *fmt, ...);

#ifdef __cplusplus
};
#endif // __cplusplus


#endif // COMMON_LOG_H
