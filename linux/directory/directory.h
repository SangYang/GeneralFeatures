#ifndef LINUX_DIRECTORY_H
#define LINUX_DIRECTORY_H


#include <dirent.h>  
#include <sys/stat.h> 
#include <sys/types.h> 

#ifndef __cplusplus
#define bool   int
#define false  0
#define true   1
#endif // __cplusplus

// @return 1)==0:continue;  2)!=0:break
typedef int (*_FtwCallback)(const char *path, const struct stat *sb, int typeflag);

// @return 1)==0:abandon;  2)==1:reserve
typedef int (*_FilterCallback)(const struct dirent *entry);

#ifdef __cplusplus
extern "C" {
#endif

	bool IsExist(const char *item);
	bool SetMode(const char *item, mode_t mode);
	bool SetOwn(const char *item, const char *owner);
	int GetFileSize(const char *path);
	void GetWorkDir(char *dir, const int size);
	bool SetWorkDir(const char *dir);
	bool GetRealPath(const char *path, char *real_path);
	bool DeleteDirTree(const char *dir);	
	bool ScanDirTree(const char *dir, _FilterCallback filtercallback);
	bool TravelDirTree(const char *dir, _FtwCallback ftwcallback);

#ifdef __cplusplus
};
#endif


#endif // LINUX_DIRECTORY_H

/*------------------------example::BEGIN--------------------------
int FtwCallback(const char *path, const struct stat *sb, int typeflag) {
	if (FTW_D == typeflag) {
		printf("%s\n", path);
	}
	else {
		printf("-----%s,%d\n", path, (int)sb->st_size);	
	}
	return 0; // 1)==0 : continue; 2)!=0 : break
}

int FilterCallback(const struct dirent *entry) {
    if (entry->d_type & DT_DIR 
		&& 0 == strcmp(entry->d_name, ".") 
		|| 0 == strcmp(entry->d_name, "..")) {
        return 0;  // abandon; 
    }
    return 1; // reserve
}
---------------------------example::END------------------------------*/

