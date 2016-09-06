#include "windows_config.h"
#include <stdio.h>
#include <string.h>
#include <windows.h>

int GetWorkDir(char *dir, int size) {
	return (int)GetCurrentDirectory(size, dir);
}

void ReadConfig(const char *confPath, t_Config *config) {
	GetPrivateProfileString("dir", "one", "", config->m_one, sizeof(config->m_one), confPath);
	config->m_two = GetPrivateProfileInt("size", "two", 0, confPath);
}

void WriteConfig(const char *confPath, const t_Config *config) {
	char two[32];
	sprintf(two, "%d", config->m_two);
	WritePrivateProfileString("dir", "one", config->m_one, confPath);
	WritePrivateProfileString("size", "two", two, confPath);
}

void PrintConfig(const t_Config *config) {
	printf("one=%s, two=%d\n", config->m_one, config->m_two);
}

#if 1
int main(void) {
	char workDir[256];
	char confPath[256];
	t_Config config = {"E:\\1.jpg", 12345};
	char one[32], two[32];

	GetWorkDir(workDir, sizeof(workDir));
	sprintf(confPath, "%s\\config2.ini", workDir);

	WriteConfig(confPath, &config);
	ReadConfig(confPath, &config);
	PrintConfig(&config);

	return 0;
}
#endif

