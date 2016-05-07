#ifndef SSY_WIN_BASIC_INFO_H
#define SSY_WIN_BASIC_INFO_H


#ifdef __cplusplus
extern "C" {
#endif

	bool GetMicroSystemInfo(char *p_systeminfo);
	bool GetCpuInfo(char *p_cpuinfo);
	void GetMemoryInfo(char *p_memoryinfo);
	void GetIpAndMac(char *p_ip, char *p_mac);

#ifdef __cplusplus
};
#endif


#endif // SSY_WIN_BASIC_INFO_H