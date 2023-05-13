#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int myfnmatch(char *str, char *pattern)
{
	if (*pattern == '\0') {
		return *str == '\0';
	} else if (*pattern == '*') {
		if (myfnmatch(str, pattern + 1))
			return 1;
		while (*str != '\0' && !myfnmatch(str + 1, pattern))
			str++;
		return *str != '\0';
	} else if (*str != '\0' && (*pattern == '?' || *str == *pattern)) {
		return myfnmatch(str + 1, pattern + 1);
	} else {
		return 0;
	}
}
