#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <getopt.h>

#include "find_min_max.h"
#include "utils.h"

int main(int argc, char **argv)
 {
  if (argc !=3)
   {
   printf("Usage: %s seed array_size\n", argv[0]);
   return 1;
   }
   char* arg_array[4];
   for(int i=0;i<4;i++) 
    {arg_array[i] = malloc(sizeof(char)*40);}
   sprintf(arg_array[0],"sequential_min_max");
   sprintf(arg_array[1], "%d", atoi(argv[1]));
   sprintf(arg_array[2], "%d", atoi(argv[2]));
   arg_array[3] = NULL;
   int pid = fork();
   if(pid == 0)
    { execv(arg_array[0], arg_array); }
   wait(&pid);
   for(int i=0; i<4;i++)
    { free(arg_array[i]);}
   return 0;
 }
