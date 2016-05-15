#include <stdio.h>
#include "include/curl.h"

int main(void) {
	printf("%s\n", curl_version());
	return 0;
}
