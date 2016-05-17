#ifndef SSY_CURL_PACKAGE_H
#define SSY_CURL_PACKAGE_H


bool Curl_Upload(const char *p_local_path, const char *p_remote_path);
bool Curl_Dwonload(const char *p_remote_path, const char *p_local_path);


#endif // SSY_CURL_PACKAGE_H