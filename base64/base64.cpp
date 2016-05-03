#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "base64.h"

#define CHANGE_ENDIAN(u)              ((u&0xff000000)>>24 | (u&0x00ff0000)>>8 | (u&0x0000ff00)<< 8 | (u&0x000000ff)<<24)
#define EN_CODE1(u)                   ((u>>26)%64)
#define EN_CODE2(u)                   ((u>>20)%64)
#define EN_CODE3(u)                   ((u>>14)%64)
#define EN_CODE4(u)                   ((u>>8)%64)
#define EN_CODE2_T(u)                 (((u>>20)%64)&0x30)
#define EN_CODE3_T(u)                 (((u>>14)%64)&0x3C)
#define COMBINE_TWO(u1, u2)           ((u1<<24) | (u2<<16))
#define COMBINE_THREE(u1, u2, u3)     ((u1<<24) | (u2<<16) | (u3<<8))
#define COMBINE_FOUR(u1, u2, u3, u4)  ((u1<<24) | (u2<<16) | (u3<<8) | u4)
#define DE_CODE1(u1, u2)              ((u1&0x3f000000)>>22 | (u2&0x00300000)>>20)
#define DE_CODE2(u1, u2)              ((u1&0x000f0000)>>12 | (u2&0x00003c00)>>10)
#define DE_CODE3(u1, u2)              ((u1&0x00000300)>>2  | (u2&0x0000003f))
#define BASE64_END_CHAR               '='

static char g_base64_table[64] = {
	'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
	'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z',
	'0','1','2','3','4','5','6','7','8','9',
	'+','/'
};

static unsigned int GetIndexFromTable(const char alphabet) {
	unsigned int index;
	for (index = 0; index < 64; index++)
		if (alphabet == g_base64_table[index])
			return index;
	return -1;
}

static int GetBase64Size(const int source_size) {
	int base64_size;
	int encode_count;
	int remainder_count;
	if (source_size <= 3)
		encode_count = source_size;
	else
		encode_count = ceil((double)source_size * 8 / 6);
	remainder_count = encode_count % 4;
	switch (remainder_count) {
		case 0:
			base64_size = encode_count;
			break;
		case 1: case 2: case 3:
			base64_size = encode_count + (4 - remainder_count);
			break;
		default:
			break;
	}
	return base64_size;
}

int Base64Encode(const char *source, int source_size, char *base64, int base64_size) {
	int src_index;
	int bas_index;
	int cycle_count;
	int remainder_count;
	int base64_size_min;
	unsigned int big_endian;
	unsigned int little_endian;

	if (!source || !base64) 
		return 0;
	else
		base64_size_min = GetBase64Size(source_size);
	if (base64_size < base64_size_min)
		return 0;
	else {
		base64[base64_size_min] = 0;
		cycle_count = source_size / 3;
		remainder_count = source_size % 3;
		src_index = 0;
		bas_index = 0;
	}
	for (; src_index < cycle_count; src_index++, bas_index++) {
		little_endian = *(unsigned int*)(source + src_index * 3);
		big_endian = CHANGE_ENDIAN(little_endian);
		base64[bas_index*4+0] = g_base64_table[EN_CODE1(big_endian)];
		base64[bas_index*4+1] = g_base64_table[EN_CODE2(big_endian)];
		base64[bas_index*4+2] = g_base64_table[EN_CODE3(big_endian)];
		base64[bas_index*4+3] = g_base64_table[EN_CODE4(big_endian)];
	}
	if (remainder_count > 0) {
		little_endian = *(unsigned int*)(source + src_index * 3);
		big_endian = CHANGE_ENDIAN(little_endian);
		if (1 == remainder_count) {
			base64[bas_index*4+0] = g_base64_table[EN_CODE1(big_endian)];
			base64[bas_index*4+1] = g_base64_table[EN_CODE2_T(big_endian)];
			base64[bas_index*4+2] = BASE64_END_CHAR;
			base64[bas_index*4+3] = BASE64_END_CHAR;
		}
		else if (2 == remainder_count) {
			base64[bas_index*4+0] = g_base64_table[EN_CODE1(big_endian)];
			base64[bas_index*4+1] = g_base64_table[EN_CODE2(big_endian)];
			base64[bas_index*4+2] = g_base64_table[EN_CODE3_T(big_endian)];
			base64[bas_index*4+3] = BASE64_END_CHAR;
		}
	}
	return 1;
}

int Base64Decode(const char *base64, int base64_size, char *source, int source_size) {
	int bas_index;	
	int src_index;
	int cycle_count;
	int remainder_count;
	int source_size_min;
	unsigned int index1, index2, index3, index4;
	unsigned int combine1, combine2;
	int end_char_count = 0;
	bool done;

	if (!base64 || !source) 
		return 0;
	else
		source_size_min = base64_size / 4 * 3;
	if (source_size < source_size_min + 1)
		return 0;
	else {
		bas_index = 0;		
		src_index = 0;
		cycle_count = base64_size / 4;
		remainder_count = base64_size % 4;
		done = false;
	}
	for (; bas_index < cycle_count; bas_index++, src_index++) {
		if (BASE64_END_CHAR == base64[bas_index*4+0])
			end_char_count++;
		if (BASE64_END_CHAR == base64[bas_index*4+1])
			end_char_count++;
		if (BASE64_END_CHAR == base64[bas_index*4+2])
			end_char_count++;
		if (BASE64_END_CHAR == base64[bas_index*4+3])
			end_char_count++;
		if ((0 == base64[bas_index*4+0]) 
			|| (0 == base64[bas_index*4+1]) 
			|| (0 == base64[bas_index*4+2]) 
			|| (0 == base64[bas_index*4+3])
			|| (end_char_count > 0))
			done = true;
		if (!done) {
			index1 = GetIndexFromTable(base64[bas_index*4+0]);
			index2 = GetIndexFromTable(base64[bas_index*4+1]);
			index3 = GetIndexFromTable(base64[bas_index*4+2]);
			index4 = GetIndexFromTable(base64[bas_index*4+3]);
			combine1 = COMBINE_FOUR(index1, index2, index3, index4);
			combine2 = combine1;
			source[src_index*3+0] = DE_CODE1(combine1, combine2);
			source[src_index*3+1] = DE_CODE2(combine1, combine2);
			source[src_index*3+2] = DE_CODE3(combine1, combine2);
		}
		else if (0 == end_char_count) {
			source[src_index*3+3] = 0;		
		}
		else if (1 == end_char_count) {
			index1 = GetIndexFromTable(base64[bas_index*4+0]);
			index2 = GetIndexFromTable(base64[bas_index*4+1]);
			index3 = GetIndexFromTable(base64[bas_index*4+2]);
			combine1 = COMBINE_THREE(index1, index2, index3);
			combine2 = combine1;
			source[src_index*3+0] = DE_CODE1(combine1, combine2);
			source[src_index*3+1] = DE_CODE2(combine1, combine2);
			source[src_index*3+2] = 0;
		}
		else if (2 == end_char_count) {
			index1 = GetIndexFromTable(base64[bas_index*4+0]);
			index2 = GetIndexFromTable(base64[bas_index*4+1]);
			combine1 = COMBINE_TWO(index1, index2);
			combine2 = combine1;
			source[src_index*3+0] = DE_CODE1(combine1, combine2);
			source[src_index*3+1] = 0;
		}
	}
	if (remainder_count > 0 && !done) {
		source[src_index*3+0] = 0;
	}
	return 1;
}



#if 0
int main() {
	char one[] = "abc";
	int  one_size;
	char one_base64[256];
	char one_base64_len;
	char two[256];

	one_size = strlen(one);
	Base64Encode(one, one_size, one_base64, 256);	
	one_base64_len = strlen(one_base64) + 1;
	Base64Decode(one_base64, one_base64_len, two, 256);
	printf("%s\n", two);

	FILE *photo;
	char *photo_buf;
	int  photo_buf_size;
	char *photo_base64;
	int  photo_base64_len;
	FILE *photo_two;
	char *photo_two_buf;
	int  photo_two_buf_len;

	photo = fopen("E:\\5_Photo\\images\\3.jpg", "rb");
	fseek(photo, 0, SEEK_END);
	photo_buf_size = ftell(photo);
	fseek(photo, 0, SEEK_SET);
	photo_buf = (char*)malloc(photo_buf_size+1);
	photo_buf[photo_buf_size] = 0;
	fread(photo_buf, sizeof(char), photo_buf_size, photo);
	photo_base64_len = photo_buf_size / 3 * 4 + 4;
	photo_base64 = (char*)malloc(photo_base64_len+1);
	photo_base64[photo_base64_len] = 0;
	Base64Encode(photo_buf, photo_buf_size, photo_base64, photo_base64_len);
	
	photo_two_buf = (char*)malloc(1024*1024+1);
	photo_two_buf[1024*1024] = 0;
	Base64Decode(photo_base64, photo_base64_len, photo_two_buf, 1024*1024);
	photo_two_buf_len = photo_buf_size;
	photo_two = fopen("123.jpg", "wb");
	fwrite(photo_two_buf, sizeof(char), photo_two_buf_len, photo_two);
	fclose(photo_two);
	free(photo_two_buf);

	free(photo_buf);
	free(photo_base64);
	fclose(photo);

	return 0;
}
#endif
