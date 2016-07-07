#include "directory.h"
#include <errno.h>
#include <ftw.h>
#include <limits.h>
#include <pwd.h>
#include <stdio.h>  
#include <stdlib.h>  
#include <string.h> 
#include <unistd.h>  

bool IsExist(const char *item) {
	if (0 == access(item, F_OK))
		return true;
	else {
		printf("[error]access(%s, F_OK)!=0,{%d:%s}\n", item, errno, strerror(errno));
		return false;		
	}
}

bool SetMode(const char *item, mode_t mode) {
	int chret;
	chret = chmod(item, mode);
	if (-1 == chret) {
		printf("[error]chmod(%s,%04o)=-1,{%d:%s}\n", item, mode, errno, strerror(errno));
		return false;
	}
	else
		return true;
}

bool SetOwn(const char *item, const char *owner) {
	struct passwd *pwd;
	int chret;

	pwd = getpwnam(owner);
	if (NULL == pwd) {
		printf("[error]getpwnam(%s)=NULL,{%d:%s}\n", owner, errno, strerror(errno));
		return false;		
	}
	else {
		chret = chown(item, pwd->pw_uid, pwd->pw_gid);
		if (-1 == chret) {
			printf("[error]chown(%s,%d,%d)=-1,{%d:%s}\n", item, pwd->pw_uid, pwd->pw_gid, errno, strerror(errno));
			return false;
		}
		else
			return true;		
	}
}

int GetFileSize(const char *path) {
	struct stat st;
	int stret;
	
	stret = stat(path, &st);
	if (-1 == stret) {
		printf("[error]stat(%s)=-1,{%d:%s}\n", path, errno, strerror(errno));	
		return -1;
	}
	else
		return st.st_size;
}

void GetWorkDir(char *dir, const int size) {
	getcwd(dir, size);
}

bool SetWorkDir(const char *dir) {
	int chret;
	
	chret = chdir(dir);
	if (-1 == chret) {
		printf("[error]chdir(%s)=-1,{%d:%s}\n", dir, errno, strerror(errno));
		return false;		
	}
	else
		return true;
}

bool GetRealPath(const char *path, char *real_path) {
	char *rearet = NULL;
	
	rearet = realpath(path, real_path);
	if (NULL == rearet) {
		printf("[error]realpath(%s)=NULL,{%d:%s}\n", path, errno, strerror(errno));
		return false;
	}
	else
		return true;
}

bool DeleteDirTree(const char *dir) {  
	DIR *dirp; 
	struct dirent *entry;   
	char childdir[PATH_MAX];
	char path[PATH_MAX];
  
	dirp = opendir(dir); 
	if (NULL != dirp) {
		entry = readdir(dirp);
		while (NULL != entry) {	
			if (DT_DIR & entry->d_type) {  
				if (0 != strcmp(entry->d_name, ".") 
					&& 0 != strcmp(entry->d_name, "..")) {
					sprintf(childdir, "%s/%s", dir, entry->d_name);				
					DeleteDirTree(childdir);	
				}
			}  
			else {
				sprintf(path, "%s/%s", dir, entry->d_name);
				remove(path);
			}
			entry = readdir(dirp);		
		}
		closedir(dirp);
		rmdir(dir);
		return true;
	}
	else {
		printf("[error]opendir(%s)=NULL,{%d:%s}\n", dir, errno, strerror(errno));
		return false;		
	}
}  

bool ScanDirTree(const char *dir, _FilterCallback filtercallback) {
	struct dirent *entry;
	struct dirent **namelist;
	char childdir[PATH_MAX];
	char path[PATH_MAX];
	int index, total;
	
	total = scandir(dir, &namelist, filtercallback, alphasort);
	if (0 > total) {
		printf("[error]scandir(%s)=%d,{%d:%s}\n", dir, total, errno, strerror(errno));
		return false;
	}
	else {
		for (index = 0; index < total; index++) {
			entry = namelist[index];
			if (DT_DIR & entry->d_type) {  
				sprintf(childdir, "%s/%s", dir, entry->d_name);
				printf("[scandir]%s\n", childdir);				
				ScanDirTree(childdir, filtercallback);
			}
			else {
				sprintf(path, "%s/%s", dir, entry->d_name);
				printf("[scandir]---%s\n", path);									
			}
			free(namelist[index]);
		}
		free(namelist);
		return true;
	}
}

bool TravelDirTree(const char *dir, _FtwCallback ftwcallback) {
	int twret;
	
	twret = ftw(dir, ftwcallback, 100);
	if (-1 == twret) {
		printf("[error]ftw(%s)=-1,{%d:%s}\n", dir, errno, strerror(errno));
		return false;
	}
	else if (0 != twret) {
		printf("[interrupt]ftw(%s)=%d\n", dir, twret);	
	}
	return true;
}

#if 1
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
        return 0;
    }
    return 1;
}

int main(int argc, char *argv[]) {  
	char workdir[PATH_MAX];
	char realpath[PATH_MAX];	
	int size;
	bool okexi;
	
	GetRealPath(".", realpath);	
	printf("[main]realpath=%s\n", realpath);			
	TravelDirTree(".", FtwCallback);
	SetMode("Log", 0777);
	SetOwn("Log", "ssyang");	
	size = GetFileSize("directory.c");
	printf("[main]size=%d\n", size);
	okexi = IsExist("directory.c");	
	printf("[main]okexi=%d\n", okexi);	
	GetWorkDir(workdir, sizeof(workdir));	
	printf("[main]workdir=%s\n", workdir);
	SetWorkDir("Log");
	GetWorkDir(workdir, sizeof(workdir));		
	printf("[main]workdir=%s\n", workdir);
	SetWorkDir("..");		
	printf("[main]workdir=%s\n", workdir);	
	ScanDirTree(workdir, FilterCallback);
	DeleteDirTree("Dir1");  

	return 0;  
}
#endif

