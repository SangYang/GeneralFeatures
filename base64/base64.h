#ifndef _BASE64_H
#define _BASE64_H
  
#ifdef __cplusplus  
extern "C" {  
#endif  
  
	int Base64_Encode(const unsigned char *source, int sourceSize, char *base64, int *base64Size);
	int Base64_Decode(const char *base64, int base64Size, unsigned char *source, int *sourceSize);
	int Base64_EncodeUrl(const char *base64, int base64Size, char *base64Url, int *base64UrlSize);
	int Base64_EncodeEmail(const char *base64, int base64Size, char *base64Email, int *base64EmailSize);

#ifdef __cplusplus  
}  
#endif  
  
#endif // _BASE64_H

