#include "revert_string.h"

void RevertString(char *str)
{
	int n = strlen(str) - 1;
	char t;
	for(int i = 0; i < n/2; ++i){
	t = str[i];
	str[i] = str[n - i];
	str[n - i] = t;
	}
}

