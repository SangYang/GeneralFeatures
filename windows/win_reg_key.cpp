#include "win_reg_key.h"
#include <stdio.h>
#include "windows.h"

static HKEY g_hkey = NULL;

bool InitRegKey(const char *p_subkey_path) {
	DWORD disposition;
	LONG regRet;

	regRet = RegCreateKeyEx(HKEY_CURRENT_USER, p_subkey_path, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &g_hkey, &disposition);
	if (ERROR_SUCCESS != regRet) {
		printf("RegCreateKeyEx() failure! regRet=%d\n", regRet);
		return false;
	}
	else {
		if (REG_CREATED_NEW_KEY == disposition)
			printf("The key did not exist and was created.\n");
		else if (REG_OPENED_EXISTING_KEY == disposition)
			printf("The key existed and was simply opened without being changed.\n");
		return true;
	}
} 

void UninitRegKey() {
	RegCloseKey(g_hkey);
}

bool SetRegKeyValue(const char *p_name, const char *p_value) {
	LONG regRet;
	DWORD value_size;

	regRet = RegSetValueEx(g_hkey, p_name, 0, REG_SZ, (BYTE *)p_value, strlen(p_value) + 1);
	if (ERROR_SUCCESS != regRet) {
		printf("RegSetValue() failure! regRet=%d\n", regRet);
		return false;
	}
	else {
		return true;
	}
}

bool GetRegKeyValue(const char *p_name, char *p_value, int value_size) {
	LONG regRet;

	regRet = RegQueryValueEx(g_hkey, p_name, NULL, NULL, (LPBYTE)p_value, (LPDWORD)&value_size);
	if (ERROR_SUCCESS != regRet) {
		printf("RegQueryValueEx() failure! regRet=%d\n", regRet);
		return false;
	}
	else {
		return true;
	}
}



#if 0
int main(void) {
	char subKey[255] = "SOFTWARE\\aaaTeamViewer";
	char value[256];
	bool ok_init;
	bool ok_get;
	bool ok_set;

	ok_init = InitRegKey(subKey);
	if (false == ok_init) {
		printf("InitRegKey() failure!\n");
	}
	else {
		printf("InitRegKey() success!\n");

		ok_set = SetRegKeyValue("abcdefg", "123456789");
		if (false == ok_set) {
			printf("SetRegKeyValue() failure!\n");
		}
		else {
			printf("SetRegKeyValue() success!\n");
		}

		ok_get = GetRegKeyValue("abcdefg", value, sizeof(value));
		if (false == ok_get) {
			printf("GetRegKeyValue() failure!\n");
		}
		else {
			printf("GetRegKeyValue() success! value=%s\n", value);
		}

		UninitRegKey();
	}

	return 0;
}
#endif 



