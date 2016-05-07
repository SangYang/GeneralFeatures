#include "win_basic_info.h"
#include <stdio.h>
#include <windows.h>
#include <iphlpapi.h>

#pragma comment(lib, "IPHlpApi.lib")

bool GetMicroSystemInfo(char *p_systeminfo) {	
	DWORD         dwType = REG_SZ;
	char          lpData[256];
	DWORD         cbData = 256;
	HKEY          hKey;					
	LPCTSTR       lpSubKey = "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion";// 取得操作系统版本信息
	LONG          regRetVal;    
	OSVERSIONINFO versionInfo;	

	regRetVal = RegOpenKeyEx(HKEY_LOCAL_MACHINE, lpSubKey, 0, KEY_READ, &hKey);
	if (regRetVal == ERROR_SUCCESS) {	
		regRetVal = RegQueryValueEx(hKey, "ProductName", NULL, &dwType, (LPBYTE)lpData, &cbData);
		if (regRetVal == ERROR_SUCCESS) {
			RegCloseKey(hKey);
			strcpy(p_systeminfo, lpData);
			ZeroMemory(&versionInfo, sizeof(OSVERSIONINFO));
			versionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);// 取得操作系统更新包信息
			if (TRUE == GetVersionEx(&versionInfo)) {
				strcat(p_systeminfo, " ");
				strcat(p_systeminfo, versionInfo.szCSDVersion);
			}
			return true;
		}
		else {
			printf("RegQueryValueEx() failure! regRetVal=%d\n", regRetVal);
			return false;
		}
	}
	else {
		printf("RegOpenKeyEx() failure! regRetVal=%d\n", regRetVal);
		return false;
	}
}

bool GetCpuInfo(char *p_cpuinfo) {
	DWORD       dwType = REG_SZ;
	char        szData[256];
	DWORD       dwData = 256;
	int         i;
	int         iTemp = 0;
	bool        bFirst = false;
	HKEY        hKey;					
	LPCTSTR     lpSubKey = "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0";// 注册表中取得 CPU 型号信息
	LONG        regRetVal;
	SYSTEM_INFO si;
	char        dwNop[5];

	regRetVal = RegOpenKeyEx(HKEY_LOCAL_MACHINE, lpSubKey, 0, KEY_READ, &hKey);
	if (ERROR_SUCCESS == regRetVal) {
		regRetVal = RegQueryValueEx(hKey, "ProcessorNameString", NULL, &dwType, (LPBYTE)szData, &dwData);
		if (regRetVal == ERROR_SUCCESS) {
			RegCloseKey(hKey);
			while (!bFirst){
				if (szData[iTemp] != ' ' && szData[iTemp] != '\t' && szData[iTemp] != '\n')
					bFirst = true;
				iTemp++;
			}
			iTemp--;

			for (i = 0; szData[iTemp] != '\0'; i++, iTemp++)
				p_cpuinfo[i] = szData[iTemp];
			p_cpuinfo[i] = szData[iTemp];

			ZeroMemory(&si, sizeof(SYSTEM_INFO));
			GetSystemInfo (&si);// CPU 核数
			_itoa (si.dwNumberOfProcessors, dwNop, 10);
			strcat(p_cpuinfo, " ");
			strcat(p_cpuinfo, dwNop);
			strcat(p_cpuinfo, "_Core");
			return true;
		}
		else {
			printf("RegQueryValueEx() failure! regRetVal=%d\n", regRetVal);
			return false;
		}
	}
	else {
		printf("RegOpenKeyEx() failure! regRetVal=%d\n", regRetVal);
		return false;
	}
}

void GetMemoryInfo(char *p_memoryinfo) {
	char         dwTp[16];
	MEMORYSTATUS ms;

	ZeroMemory(&ms, sizeof(MEMORYSTATUS));
	GlobalMemoryStatus (&ms);
	_itoa (int(double(ms.dwTotalPhys) / 1024 / 1024 / 1024 + 0.5), dwTp, 10);// （单位：B 转换为 GB）
	strcpy(p_memoryinfo, dwTp);
	strcat(p_memoryinfo, " GB");	
}

void GetIpAndMac(char *p_ip, char *p_mac) {
	// （需 Iphlpapi.h 和 IPHlpApi.Lib）
	PIP_ADAPTER_INFO pAdapterInfo;
	PIP_ADAPTER_INFO pAdapter = NULL;
	ULONG            ulOutBufLen = sizeof (IP_ADAPTER_INFO) * 4;
	DWORD            dwRetVal = 0;

	pAdapterInfo = (IP_ADAPTER_INFO *)malloc(ulOutBufLen);
	dwRetVal     = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen);
	if (dwRetVal == NO_ERROR){	// 取得适配器信息
		pAdapter = pAdapterInfo;
		while (pAdapter){		// 遍历适配器
			if (pAdapter->Type == MIB_IF_TYPE_ETHERNET){ // 以太网	
				// IP
				strcpy(p_ip, pAdapter->IpAddressList.IpAddress.String);
				// MAC
				for (UINT i = 0, j = 0; i < pAdapter->AddressLength && j < 65; i++, j+=3) {
					char adl[3];
					if (i == (pAdapter->AddressLength - 1)){
						sprintf(adl, "%.2X", (int) pAdapter->Address[i]);
						p_mac[j] = adl[0];
						p_mac[j+1] = adl[1];
						p_mac[j+2] = '\0';
					}
					else {
						sprintf(adl, "%.2X", (int) pAdapter->Address[i]);
						p_mac[j] = adl[0];
						p_mac[j+1] = adl[1];
						p_mac[j+2] = '-';
					}
				}
				break;
			}	
			pAdapter = pAdapter->Next;
		}
	}
	if (pAdapterInfo)
		free (pAdapterInfo);
}


#if 0
int main(void) {
	char systeminfo[256];
	char cpuinfo[256];
	char memoryinfo[16];
	char ip[32];
	char mac[32];

	GetMicroSystemInfo(systeminfo);
	GetCpuInfo(cpuinfo);
	GetMemoryInfo(memoryinfo);
	GetIpAndMac(ip, mac);
	printf("%s\n%s\n%s\n%s\n%s\n", systeminfo, cpuinfo, memoryinfo, ip, mac);

	return 0;
}
#endif


