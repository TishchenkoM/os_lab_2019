#include "revert_string.h"
#include <string.h>

void RevertString(char *str)
{
	int size = strlen(str)-1;
    int i;
    char elem;
    for (i=0; i<(size+1)/2; i++)
    {
        elem = str[i];
        str[i] = str[size-i];
        str[size-i] = elem;
    }
}

