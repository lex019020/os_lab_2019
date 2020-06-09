#include "revert_string.h"

void RevertString(char *str)
{
	int n = strlen(str);
	char t;
	for(int i = 0; i < n/4; ++i){
	t = str[i];
	str[i] = str[n/2 - i - 1];
	str[n/2 - i - 1] = t;
	}
}

