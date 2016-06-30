#ifndef COMMON_LOG_H
#define COMMON_LOG_H


#define USE_LOGMACRO_STANDARD
#define USE_LOGTIME_STYLE_SIMPLE

#if defined(_WIN32) && defined(USE_LOGMACRO_STANDARD)
#define LOGDEBUG(fmt, ...)   Log_Write(LOG_STYLE_DEBUG, "[%s|%d|%s] "##fmt, Log_GetFileName(__FILE__), __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define LOGINFO(fmt, ...)    Log_Write(LOG_STYLE_INFO, "[%s|%d|%s] "##fmt, Log_GetFileName(__FILE__), __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define LOGWARN(fmt, ...)    Log_Write(LOG_STYLE_WARN, "[%s|%d|%s] "##fmt, Log_GetFileName(__FILE__), __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define LOGERROR(fmt, ...)   Log_Write(LOG_STYLE_ERROR, "[%s|%d|%s] "##fmt, Log_GetFileName(__FILE__), __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define LOGFATAL(fmt, ...)   Log_Write(LOG_STYLE_FATAL, "[%s|%d|%s] "##fmt, Log_GetFileName(__FILE__), __LINE__, __FUNCTION__, ##__VA_ARGS__)
#elif defined(_WIN32) && defined(USE_LOGMACRO_SIMPLE)
#define LOGDEBUG(fmt, ...)   Log_Write(LOG_STYLE_DEBUG, ##fmt, ##__VA_ARGS__)
#define LOGINFO(fmt, ...)    Log_Write(LOG_STYLE_INFO, ##fmt, ##__VA_ARGS__)
#define LOGWARN(fmt, ...)    Log_Write(LOG_STYLE_WARN, ##fmt, ##__VA_ARGS__)
#define LOGERROR(fmt, ...)   Log_Write(LOG_STYLE_ERROR, ##fmt, ##__VA_ARGS__)
#define LOGFATAL(fmt, ...)   Log_Write(LOG_STYLE_FATAL, ##fmt, ##__VA_ARGS__)
#elif defined(__linux) && defined(USE_LOGMACRO_STANDARD)
#define LOGDEBUG(fmt, ...)   Log_Write(LOG_STYLE_DEBUG, "[%s|%d|%s] "fmt, Log_GetFileName(__FILE__), __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define LOGINFO(fmt, ...)    Log_Write(LOG_STYLE_INFO, "[%s|%d|%s] "fmt, Log_GetFileName(__FILE__), __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define LOGWARN(fmt, ...)    Log_Write(LOG_STYLE_WARN, "[%s|%d|%s] "fmt, Log_GetFileName(__FILE__), __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define LOGERROR(fmt, ...)   Log_Write(LOG_STYLE_ERROR, "[%s|%d|%s] "fmt, Log_GetFileName(__FILE__), __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define LOGFATAL(fmt, ...)   Log_Write(LOG_STYLE_FATAL, "[%s|%d|%s] "fmt, Log_GetFileName(__FILE__), __LINE__, __FUNCTION__, ##__VA_ARGS__)
#elif defined(__linux) && defined(USE_LOGMACRO_SIMPLE)
#define LOGDEBUG(fmt, ...)   Log_Write(LOG_STYLE_DEBUG, fmt, ##__VA_ARGS__)
#define LOGINFO(fmt, ...)    Log_Write(LOG_STYLE_INFO, fmt, ##__VA_ARGS__)
#define LOGWARN(fmt, ...)    Log_Write(LOG_STYLE_WARN, fmt, ##__VA_ARGS__)
#define LOGERROR(fmt, ...)   Log_Write(LOG_STYLE_ERROR, fmt, ##__VA_ARGS__)
#define LOGFATAL(fmt, ...)   Log_Write(LOG_STYLE_FATAL, fmt, ##__VA_ARGS__)
#else
#define LOGDEBUG(fmt, ...)   ((void *)0)
#define LOGINFO(fmt, ...)    ((void *)0)
#define LOGWARN(fmt, ...)    ((void *)0)
#define LOGERROR(fmt, ...)   ((void *)0)
#define LOGFATAL(fmt, ...)   ((void *)0)
#endif // _WIN32 && __linux && USE_*

typedef enum {
	LOGTIME_STYLE_STANDARD,
	LOGTIME_STYLE_SIMPLE,
	LOGTIME_STYLE_YEAR,
	LOGTIME_STYLE_MONTH,
	LOGTIME_STYLE_DAY,
	LOGTIME_STYLE_HOUR,
	LOGTIME_STYLE_MINUTE,
	LOGTIME_STYLE_SECOND
} e_LogtimeStyle;

typedef enum {
	LOG_STYLE_DEBUG = 10,
	LOG_STYLE_INFO,
	LOG_STYLE_WARN,
	LOG_STYLE_ERROR,
	LOG_STYLE_FATAL
} e_LogStyle;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

	const char* Log_GetFileName(const char *p_path);
	int Log_Init(const char *p_path, e_LogtimeStyle logtime_style);
	int Log_Write(const e_LogStyle logstyle, const char *p_format, ...);

#ifdef __cplusplus
};
#endif // __cplusplus


#endif // COMMON_LOG_H
