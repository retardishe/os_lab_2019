#include "revert_string.h"
#include <string.h>
#include <stddef.h>

void RevertString(char *str)
{
	char temp;
	int len = strlen(str);
	for (size_t i = 0; i< len/2; i++) {
		temp = str[i];
		str[i] = str[len-i-1];
		str[len-i-1] = temp;
	}
	return;
}

