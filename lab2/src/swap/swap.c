#include "swap.h"
#include <stdio.h>

void Swap(char *left, char *right)
{
 char therd = *left;
 *left = *right;
 *right = therd; 
}
