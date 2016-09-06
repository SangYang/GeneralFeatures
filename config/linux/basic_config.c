#include "basic_config.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

void GetPathFromPid(int pid, char *path, int size) {
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

static char* MallocFileBuff(const char *path) {
	FILE *file = NULL;
	char *buffer = NULL;
	struct stat st;

	file = fopen(path, "rb");
	if (NULL != file) {
		stat(path, &st);
		buffer = (char *)malloc(st.st_size+1);
		buffer[st.st_size] = 0;
		fread(buffer, sizeof(char), st.st_size, file);
		fclose(file);
	}	
	return buffer;
}

static void FreeFileBuff(const char *buffer) {
	free((char *)buffer);
}

static void ReviseRetString(char *retString) {
	int index;
	index = strspn(retString, " ");
	if (0 < index)
		memmove(retString, retString+index, strlen(retString+index)+1);
	for (index = strlen(retString)-1; index >= 0; index--) {
		if (' ' == retString[index])
			retString[index] = 0;
		else 
			break;
	}
}

int GetPrivateProfileString(const char *appName, const char *keyName, const char *defaultValue, 
							char *retString, int retSize, const char *filePath) {
	char *fileBuff = NULL;
	char *appStart = NULL, *appEnd = NULL;
	char *keyStart = NULL;
	char *valueStart = NULL, *valueEnd = NULL;
	char app[32];
	int retLen = -1;

	if (NULL == appName || NULL == keyName || NULL == filePath) 
		return retLen;

	sprintf(app, "[%s]", appName);
	fileBuff = MallocFileBuff(filePath);
	if (NULL != fileBuff) {
		appStart = strstr(fileBuff, app);
		if (NULL != appStart) {
			appEnd = strchr(appStart+1, '[');
			keyStart = strstr(appStart, keyName);
			if ((NULL != keyStart && NULL != appEnd && keyStart < appEnd) 
				|| (NULL != keyStart && NULL == appEnd)) {
				valueStart = strchr(keyStart, '=');
				valueEnd = strpbrk(keyStart, "\r\n");
				if (NULL != valueStart && NULL == valueEnd) {
					retLen = strcspn(valueStart, "\r\n");
					if (1 == retLen) 
						strcpy(retString, "");
					else {
						valueStart++;
						retLen--;
						if (retSize > retLen) {
							strncpy(retString, valueStart, retLen);
							retString[retLen] = 0;
							ReviseRetString(retString);
							retLen = strlen(retString);							
						}
						else
							retLen = -1;
					}
				}
				else if (NULL != valueStart && NULL != valueEnd && 0 < (valueEnd-valueStart)) {
					valueStart++;
					retLen = valueEnd - valueStart;
					if (retSize > retLen) {
						strncpy(retString, valueStart, retLen);
						retString[retLen] = 0;
						ReviseRetString(retString);
						retLen = strlen(retString);
					}
					else
						retLen = -1;
				}
			}
		}
		FreeFileBuff(fileBuff);
	}
	if (-1 == retLen) { 
		if (NULL == defaultValue) {
			strcpy(retString, "");
			retLen = 0;
		}
		else if (retSize > strlen(defaultValue)) {
			strcpy(retString, defaultValue);
			ReviseRetString(retString);
			retLen = strlen(retString);
		}
	}
	return retLen;
}

static int IsDigitForStr(const char *str) {
	int index;
	for (index = 0; index < strlen(str); index++) {
		if (0 == isdigit(str[index]))
			return 0;
	}
	return 1;
}

int GetPrivateProfileInt(const char *appName, const char *keyName, int defaultValue, const char *filePath) {
	char defValue[32];
	char retString[32];
	int strRet;
	int retInt = -1;

	sprintf(defValue, "%d", defaultValue);
	strRet = GetPrivateProfileString(appName, keyName, defValue, retString, sizeof(retString), filePath);
	if (-1 == strRet || 0 == strRet || 0 == IsDigitForStr(retString)) 
		retInt = defaultValue;
	else
		retInt = atoi(retString);
	return retInt;
}

static void SaveFileBuff(const char *path, const char *buffer) {
	FILE *file = fopen(path, "wb");
	if (NULL != file) {
		fwrite(buffer, sizeof(char), strlen(buffer), file);
		fclose(file);
	}
}

int WritePrivateProfileString(const char *appName, const char *keyName, const char *valueString, const char *filePath) {
	char *oldBuff = NULL, *newBuff = NULL;
	char *appStart = NULL, *appEnd = NULL;
	char *keyStart = NULL,  *valueEnd = NULL;
	char app[64], value[256];
	int addSize;
	int retLen = -1;

	if (NULL == appName || NULL == keyName || NULL == filePath) 
		return retLen;
	sprintf(app, "[%s]", appName);
	if (NULL == valueString)
		strcpy(value, "");
	else
		strcpy(value, valueString);
	addSize = strlen(appName) + strlen(keyName) + strlen(value) + 32;
	oldBuff = MallocFileBuff(filePath);
	if (NULL != oldBuff) {
		newBuff = (char *)malloc(strlen(oldBuff)+addSize);

		appStart = strstr(oldBuff, app);
		if (NULL == appStart) {
			sprintf(newBuff, "%s\r\n[%s]\r\n%s=%s", oldBuff, appName, keyName, value);
			retLen = 1;
		}
		else {
			appEnd = strchr(appStart+1, '[');
			keyStart = strstr(appStart, keyName);
			if ((NULL != keyStart && NULL != appEnd && keyStart < appEnd) 
				|| (NULL != keyStart && NULL == appEnd)) {
				strncpy(newBuff, oldBuff, keyStart-oldBuff);
				newBuff[keyStart-oldBuff] = 0;
				valueEnd = strpbrk(keyStart, "\r\n");
				if (NULL == valueEnd) 
					sprintf(newBuff, "%s%s=%s", newBuff, keyName, value);
				else
					sprintf(newBuff, "%s%s=%s%s", newBuff, keyName, value, valueEnd);
				retLen = 2;
			}
			else if (NULL != appEnd && NULL == keyStart) {
				strncpy(newBuff, oldBuff, appEnd-oldBuff);
				newBuff[appEnd-oldBuff] = 0;
				sprintf(newBuff, "%s%s=%s\r\n%s", newBuff, keyName, value, appEnd);	
				retLen = 3;
			}
			else if (NULL == appEnd && NULL == keyStart) {
				sprintf(newBuff, "%s\r\n%s=%s", oldBuff, keyName, value);
				retLen = 4;
			}
		}
		if (-1 != retLen)
			SaveFileBuff(filePath, newBuff);	

		free(newBuff);
		FreeFileBuff(oldBuff);
	}	
	else {
		newBuff = (char *)malloc(addSize);
		sprintf(newBuff, "[%s]\r\n%s=%s", appName, keyName, value);
		SaveFileBuff(filePath, newBuff);
		free(newBuff);
		retLen = 5;
	}
	return retLen;
}

#if 0
int main(void) {
	char workDir[256];
	char confPath[256];
	char one[256];

	GetWorkDir(workDir, sizeof(workDir));
	sprintf(confPath, "%s/config.ini", workDir);
	printf("confPath=%s\n", confPath);

	int ccc = WritePrivateProfileString("size", "two", "12345", confPath);
	int ddd = WritePrivateProfileString("dir", "one", "./aaa.jpg", confPath);
	printf("ccc=%d, ddd=%d\n", ccc, ddd);
	int aaa = GetPrivateProfileString("dir", "one", NULL, one, sizeof(one), confPath);
	int bbb = GetPrivateProfileInt("size", "two", 1111, confPath);
	printf("aaa=%d, one=%s, bbb=%d\n", aaa, one, bbb);

	return 0;
}
#endif

