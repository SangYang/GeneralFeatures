#ifndef _BASE64_H
#define _BASE64_H
  
#ifdef __cplusplus  
extern "C" {  
#endif  
  
int Base64Encode(const unsigned char *source, int source_size, char *base64, int *base64_size);
int Base64Decode(const char *base64, int base64_size, unsigned char *source, int *source_size);

#ifdef __cplusplus  
}  
#endif  
  
#endif // _BASE64_H