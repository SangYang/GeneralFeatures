#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mbstring.h>
#include "jpg.h"

static bool CheckJPGMeasure(const unsigned char *buffer, int size) {
	unsigned char *find_info = NULL;
	int  pos_info = 0;
	int  width = 0;
	int  height = 0;

	if (!buffer || !_mbscmp(buffer, (unsigned char*)("")) || 0 == size)
		return false;

	find_info = (unsigned char*)memchr(buffer, 0xFF, size);
	while (find_info) {
		if (0xC0 == *(find_info+1)) {
			height = 256 * (*(find_info+5)) + (*(find_info+6));
			width = 256 * (*(find_info+7)) + (*(find_info+8));
			break;
		}
		else {
			pos_info = find_info - buffer;
			find_info = (unsigned char*)memchr(find_info+1, 0xFF, size-pos_info-1);
		}
	}

	return ((4096 > width && 799 < width) && (4096 > height && 799 < height)); // (799~4400)
}


static bool CheckJPGFormat(const unsigned char *buffer, int size) {
	unsigned char *find_head = NULL;
	unsigned char *find_tail = NULL;
	int  pos_head = 0;
	int  pos_tail = 0;
	bool is_head = false;
	bool is_tail = false;

	if (!buffer || !_mbscmp(buffer, (unsigned char*)("")) || 0 == size)
		return false;

	find_head = (unsigned char*)memchr(buffer, 0xFF, size);
	while (find_head) {
		if (0xD8 == *(find_head+1)) {
			is_head = true;
			break;
		}
		else {
			pos_head = find_head - buffer;
			find_head = (unsigned char*)memchr(find_head+1, 0xFF, size-pos_head-1);
		}
	}

	if (true == is_head) {
		find_tail = (unsigned char*)memchr(find_head+1, 0xFF, size-pos_head-1);
		while (find_tail) {
			if (0xD9 == *(find_tail+1)) {
				is_tail = true;
				break;
			}
			else {
				pos_tail = find_tail - buffer;
				find_tail = (unsigned char*)memchr(find_tail+1, 0xFF, size-pos_tail-1);
			}
		}
	}

	return (is_head && is_tail);
}

bool IsRightJPG(const char *file_name) {
	FILE *file = NULL;
	unsigned char *buffer = NULL;
	int  size = 0;
	bool is_right_format = false;
	bool is_right_measure = false;

	if (!file_name || !strcmp(file_name, ""))
		return false;
	else
		file = fopen(file_name, "rb");
	if (!file)
		return false;
	else {
		fseek(file, 0, SEEK_END);
		size = ftell(file);
		fseek(file, 0, SEEK_SET);
	}

	buffer = (unsigned char*)malloc(size+1);
	buffer[size] = '\0';
	fread(buffer, sizeof(unsigned char), size, file);
	is_right_format = CheckJPGFormat(buffer, size);
	is_right_measure = CheckJPGMeasure(buffer, size);
	free(buffer);
	fclose(file);

	return (is_right_format && is_right_measure);
}
