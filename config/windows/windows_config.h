#ifndef SSY_WINDOWS_CONFIG_H
#define SSY_WINDOWS_CONFIG_H


typedef struct {
	char m_one[32];
	int  m_two;
} t_Config;

#ifdef __cplusplus
extern "C" {
#endif

	int GetWorkDir(char *dir, int size);
	void ReadConfig(const char *confPath, t_Config *config);
	void WriteConfig(const char *confPath, const t_Config *config);
	void PrintConfig(const t_Config *config);

#ifdef __cplusplus
};
#endif


#endif // SSY_WINDOWS_CONFIG_H

