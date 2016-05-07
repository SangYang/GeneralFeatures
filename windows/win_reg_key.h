#ifndef SSY_WIN_REG_KEY_H
#define SSY_WIN_REG_KEY_H


bool InitRegKey(const char *p_subkey_path);
void UninitRegKey();
bool SetRegKeyValue(const char *p_name, const char *p_value);
bool GetRegKeyValue(const char *p_name, char *p_value, int value_size);


#endif // SSY_WIN_REG_KEY_H
