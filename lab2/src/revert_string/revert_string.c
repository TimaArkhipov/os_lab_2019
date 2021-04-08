#include "revert_string.h"
#include <string.h>
#include <stdlib.h>

void RevertString(char* str)
{
	char* str1 = (char*)malloc(sizeof(char) * (strlen(str) + 1));
	for (int i = 0; i < strlen(str); i++)
	{
		str1[i] = str[strlen(str) - i - 1];
	}
	str1[strlen(str)] = '\0';
	strcpy(str, str1);
	free(str1);
}

