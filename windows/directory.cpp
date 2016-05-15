#include "directory.h"
#include <io.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <Windows.h>
#include "transcode.h"

#define ASSERT(b)  NULL

void ModifyPath(const char *p_path, char *p_modPath, const int modSize) {
	char *p_tok = NULL;
	char tok[256] = {0};

	ASSERT(NULL != p_path && NULL != p_modPath);
	ASSERT(modSize > strlen(p_path));
	strcpy(p_modPath, p_path);
	p_tok = strtok(p_modPath, "\\");
	while (NULL != p_tok) {
		strcat(tok, p_tok);
		strcat(tok, "\\\\");
		p_tok = strtok(NULL, "\\");
	}
	ASSERT(modSize > strlen(tok));
	strncpy(p_modPath, tok, strlen(tok) - 2);
	p_modPath[strlen(tok) - 2] = '\0'; 
}

bool GetFileSize(const char *p_path, int *p_size) {
	FILE *pf = NULL;
	int size;

	ASSERT(NULL != p_path && NULL != p_size);
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


bool GetFileBuffer(const char *p_path, unsigned char *p_buffer, const int buffSize) {
	FILE *pf = NULL;
	int fileSize;
	bool result;
	
	ASSERT(NULL != p_path && NULL != p_buffer);
	pf = fopen(p_path, "rb"); 
	if (NULL == pf) {
		printf("fopen() error! path=%s\n", p_path);
		result = false;
	}
	else {
		GetFileSize(p_path, &fileSize);
		if (0 == fileSize || buffSize < fileSize) {
			printf("GetFileBuffer() error! file_size=%d, buffer_size=%d\n", fileSize, buffSize);
			result = false;
		}
		else {
			fread(p_buffer, sizeof(unsigned char), fileSize, pf);
			result = true;
		}
		fclose(pf);
	}
	return result;
}

bool CreateFileFromBuff(const char *p_path, const unsigned char *p_buffer, const int buffSize) {
	FILE *pf = NULL;
	bool result;

	ASSERT(NULL != p_path);
	pf = fopen(p_path, "wb"); 
	if (NULL == pf) {
		printf("fopen() error! path=%s\n", p_path);
		result = false;
	}
	else {
		if (NULL == p_buffer || 0 == buffSize) {
			// nothing
		}
		else {
			fwrite(p_buffer, sizeof(unsigned char), buffSize, pf);
		}
		fclose(pf);
		result = true;
	}
	return result;
}


void CheckDirectory(const char *p_path) {
	char drive[32];
	char dir[256];
	char folder[256];
	char tok[256] = {0};
	char *p_tok = NULL;

	ASSERT(NULL != p_path);
	ASSERT(strlen(p_path) < sizeof(dir));
	_splitpath(p_path, drive, dir, NULL, NULL);
	sprintf(folder, "%s%s", drive, dir);
	p_tok = strtok(folder, "\\");
	while (NULL != p_tok) {
		strcat(tok, p_tok);
		strcat(tok, "\\");
		//printf("%s\n", tok);
		CreateDirectoryA(tok, NULL);
		p_tok = strtok(NULL, "\\");
	}
}

bool CopyFile(const char *p_srcPath, const char *p_destPath) {
	FILE *pf_src = NULL;
	FILE *pf_dest = NULL;
	unsigned char *p_buffer = NULL;
	int size;
	bool result;

	ASSERT(NULL != p_srcPath && NULL != p_destPath);
	pf_src = fopen(p_srcPath, "rb");
	if (NULL == pf_src) {
		printf("fopen() error! path=%s\n", p_srcPath);
		result = false;
	}
	else {
		CheckDirectory(p_destPath);
		GetFileSize(p_srcPath, &size);
		if (0 == size) {
			result = CreateFileFromBuff(p_destPath, NULL, 0);
		}
		else {
			p_buffer = (unsigned char *)malloc(size + 1);
			ASSERT(NULL != p_buffer);
			fread(p_buffer, sizeof(unsigned char), size, pf_src);
			result = CreateFileFromBuff(p_destPath, p_buffer, size);
			free(p_buffer);
		}
		fclose(pf_src);
	}

	return result;
}

bool TraversalFolder(const char *p_srcDir, const char *p_destDir, const char *p_srcName, char *p_destName, const int mode) {
	struct _finddata_t fileinfo;
	char srcPath[256];
	char destPath[256];
	char destName[256];
	long first_ret;
	int next_ret;
	int file_index;

	ASSERT(NULL != p_srcDir);
	strcpy(srcPath, p_srcDir);
	strcat(srcPath, "\\");
	strcat(srcPath, p_srcName);
	first_ret = _findfirst(srcPath, &fileinfo);
	if (-1L == first_ret) {
		printf("_findfirst() error! path=%s\n", srcPath);
		return false;
	}
	else {
		file_index = 0;
		next_ret = _findnext(first_ret, &fileinfo);
		while (-1 != next_ret) {
			switch (mode) {
				case PRINT_MODE:
					printf("name=%s\n", fileinfo.name);
					break;
				case REMOVE_MODE:
					sprintf(srcPath, "%s\\%s", p_srcDir, fileinfo.name);
					if (_A_SUBDIR == fileinfo.attrib) // 文件夹
						RemoveDirectoryA(srcPath);
					else
						remove(srcPath);
					break;
				case RENAME_MODE:
					if (_A_SUBDIR == fileinfo.attrib) {// 文件夹
						//nothing
					}
					else {
						sprintf(srcPath, "%s\\%s", p_srcDir, fileinfo.name);
						sprintf(destName, "%s_%d", fileinfo.name, file_index);
						sprintf(destPath, "%s\\%s", p_srcDir, destName);
						rename(srcPath, destPath);
					}				
					break;
				case COPY_MODE:
					if (_A_SUBDIR == fileinfo.attrib) {// 文件夹
						//nothing
					}
					else {
						sprintf(srcPath, "%s\\%s", p_srcDir, fileinfo.name);
						//sprintf(destName, "%s_%d", "abc", file_index);
						sprintf(destName, "%s_%d", fileinfo.name, file_index);
						sprintf(destPath, "%s\\%s", p_destDir, destName);
						CopyFile(srcPath, destPath);
					}
					break;
				default:
					break;
			}
			next_ret = _findnext(first_ret, &fileinfo);
			file_index++;
		}
		_findclose(first_ret);
		return true;
	}
}

bool WTraversalFolder(const char *p_srcDir, const char *p_destDir, const char *p_srcName, char *p_destName, const int mode) {
	struct _wfinddata_t fileinfo;
	char srcPath[256];
	char destPath[256];
	char destName[256];
	wchar_t wdestName[256];
	wchar_t wsrcPath[256];
	wchar_t wdestPath[256];
	long first_ret;
	int next_ret;
	int file_index;

	ASSERT(NULL != p_srcDir);
	strcpy(srcPath, p_srcDir);
	strcat(srcPath, "\\");
	strcat(srcPath, p_srcName);
	ChToWch(srcPath, wsrcPath, sizeof(wsrcPath));
	first_ret = _wfindfirst(wsrcPath, &fileinfo);
	if (-1L == first_ret) {
		printf("_wfindfirst() error! path=%s\n", srcPath);
		return false;
	}
	else {
		file_index = 0;
		next_ret = _wfindnext(first_ret, &fileinfo);
		while (-1 != next_ret) {
			switch (mode) {
				case PRINT_MODE:
					printf("index=%d, name=%ls\n", file_index, fileinfo.name);
					break;
				case REMOVE_MODE:
					ChToWch(p_srcDir, wsrcPath, sizeof(wsrcPath));
					wcscat(wsrcPath, L"\\");
					wcscat(wsrcPath, fileinfo.name);
					//WchToCh(wsrcPath, srcPath, sizeof(srcPath));

					if (_A_SUBDIR == fileinfo.attrib) // 文件夹
						RemoveDirectoryW(wsrcPath);
					else
						_wremove(wsrcPath);
					break;
				case RENAME_MODE:
					if (_A_SUBDIR == fileinfo.attrib) {// 文件夹
						//nothing
					}
					else {
						ChToWch(p_srcDir, wsrcPath, sizeof(wsrcPath));
						wcscat(wsrcPath, L"\\");
						wcscat(wsrcPath, fileinfo.name);

						ChToWch(p_srcDir, wdestPath, sizeof(wdestPath));
						wcscat(wdestPath, L"\\");
						swprintf(wdestName, L"%ls_%d", fileinfo.name, file_index);
						wcscat(wdestPath, wdestName);

						_wrename(wsrcPath, wdestPath);
					}				
					break;
				case COPY_MODE:
					if (_A_SUBDIR == fileinfo.attrib) {// 文件夹
						//nothing
					}
					else {
						ChToWch(p_srcDir, wsrcPath, sizeof(wsrcPath));
						wcscat(wsrcPath, L"\\");
						wcscat(wsrcPath, fileinfo.name);
						WchToCh(wsrcPath, srcPath, sizeof(srcPath));

						sprintf(destName, "%ls_%d", fileinfo.name, file_index);
						sprintf(destPath, "%s\\%s", p_destDir, destName);
						CopyFile(srcPath, destPath);
					}
					break;
				default:
					break;
			}
			next_ret = _wfindnext(first_ret, &fileinfo);
			file_index++;
		}
		_findclose(first_ret);
		return true;
	}
}


#if 1
int main(void) {
	char path[256] = "D:\\photo\\abc\\123.jpg";
	char modPath[256];
	int file_size;

	ModifyPath(path, modPath, sizeof(modPath));
	//printf("%s\n", modPath);

	WTraversalFolder("E:\\test", "E:\\test2\\aa\\bb\\cc", "*", "", COPY_MODE);

	getchar();
	return 0;
}
#endif
