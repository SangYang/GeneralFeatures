#ifndef SSY_BASIC_CONFIG_H
#define SSY_BASIC_CONFIG_H


#ifdef __cplusplus
extern "C" {
#endif

	void GetPathFromPid(int pid, char *path, int size);
	int GetPrivateProfileString(const char *appName, const char *keyName, const char *defaultValue, 
								char *retString, int retSize, const char *filePath);
	int GetPrivateProfileInt(const char *appName, const char *keyName, int defaultValue, const char *filePath);
	int WritePrivateProfileString(const char *appName, const char *keyName, const char *valueString, const char *filePath);

#ifdef __cplusplus
};
#endif


#endif // SSY_BASIC_CONFIG_H

