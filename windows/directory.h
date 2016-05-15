#ifndef SSY_DIRECTORY_H
#define SSY_DIRECTORY_H


#define REMOVE_MODE   101
#define RENAME_MODE   102
#define COPY_MODE     103
#define PRINT_MODE    104


void ModifyPath(const char *p_path, char *p_modPath, const int modSize);
bool GetFileSize(const char *p_path, int *p_size);
bool GetFileBuffer(const char *p_path, unsigned char *p_buffer, const int buffSize);
bool CreateFileFromBuff(const char *p_path, const unsigned char *p_buffer, const int buffSize);
void CheckDirectory(const char *p_path);
bool CopyFile(const char *p_srcPath, const char *p_destPath);
bool TraversalFolder(const char *p_srcDir, const char *p_destDir, const char *p_srcName, char *p_destName, const int mode);
bool WTraversalFolder(const char *p_srcDir, const char *p_destDir, const char *p_srcName, char *p_destName, const int mode);


#endif // SSY_DIRECTORY_H

