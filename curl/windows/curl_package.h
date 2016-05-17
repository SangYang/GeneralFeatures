#ifndef SSY_CURL_PACKAGE_H
#define SSY_CURL_PACKAGE_H


bool ftp_upload(const char *p_local_path, const char *p_remote_path);
bool ftp_download(const char *p_remote_path, const char *p_local_path);


#endif // SSY_CURL_PACKAGE_H