#define _POSIX_C_SOURCE 1
#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <getopt.h>

#include "find_min_max.h"
#include "utils.h"

int pnum;
pid_t* ch_pid;

void alarm_s()
 {
  for(int i=0; i<pnum;i++) 
   {kill(ch_pid[i], SIGQUIT);}
  }

int main(int argc, char **argv) {
  int seed = -1;
  int array_size = -1;
  pnum = -1;
  bool with_files = false;
  int timeout = -1;


  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"seed", required_argument, 0, 0},
                                      {"array_size", required_argument, 0, 0},
                                      {"pnum", required_argument, 0, 0},
				      {"timeout", optional_argument, 0 ,0},
                                      {"by_files", no_argument, 0, 'f'},
				      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "f", options, &option_index);

    if (c == -1) break;

    switch (c) {
      case 0:
        switch (option_index) {
          case 0:
            seed = atoi(optarg);
            // your code here
            if(seed<=0)
		{
		printf("seed is negative\n");
		return 1;
		}
            break;
          case 1:
            array_size = atoi(optarg);
            // your code here
            if(array_size <= 0)
		{
		printf("array_size is negative\n");
		return 1;
		}
            break;
          case 2:
            pnum = atoi(optarg);
            // your code here
            if(pnum<=0)
		{	
		printf("pnum is negative\n");
		return 1;
		}
            break;
          case 3:
            timeout = atoi(optarg);
		if(timeout<=0)
		{printf("timeout is negative\n");return 1;}
            break;

          defalut:
            printf("Index %d is out of options\n", option_index);
        }
        break;
      case 'f':
        with_files = true;
        break;

      case '?':
        break;

      default:
        printf("getopt returned character code 0%o?\n", c);
    }
  }

  if (optind < argc) {
    printf("Has at least one no option argument\n");
    return 1;
  }

  if (seed == -1 || array_size == -1 || pnum == -1) {
    printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\" \n",
           argv[0]);
    return 1;
  }

  int *array = malloc(sizeof(int) * array_size);
  GenerateArray(array, array_size, seed);
  for(int j = 0; j<array_size / pnum; j++)
  {printf("%d ",array[j]);}

  int active_child_processes = 0;

  struct timeval start_time;
  gettimeofday(&start_time, NULL);
 
  int step = array_size / pnum;
  int last_step = array_size % pnum;
  if(last_step == 0) {last_step = step;}
  else {step++;} 
 
  int pipe_fd[2];
  if(!with_files)
  {pipe(pipe_fd);}

  ch_pid=malloc(sizeof(pid_t)*pnum);

  for (int i = 0; i < pnum; i++) {
    pid_t child_pid = fork();
    ch_pid[i] = child_pid;
    int step_i;
    if (i < pnum - 1){step_i = step;}
    else{step_i = last_step;}
    if (child_pid >= 0) {
      // successful fork
      active_child_processes += 1;
      if (child_pid == 0) {
        // child process
	struct MinMax *min_max = malloc(sizeof(struct MinMax));
	min_max[0] = GetMinMax(array, i*step, i*step+step_i);

        if (with_files) {
          // use files here
	char path[10];
	sprintf(path,"%d.bin",i);
	FILE *min_max_file;
	if ((min_max_file = fopen(path, "wb")) == NULL)
	{printf("Error openning file for reading"); return 1;}
	if(fwrite(min_max,sizeof(struct MinMax),1,min_max_file)!=1)
	{printf("Error writting into file"); return 1;}
	fclose(min_max_file); 
        } else {
          // use pipe here
	write(pipe_fd[1], min_max, sizeof(struct MinMax));
        }
	free(min_max);
	return 0;
      }
      if(!with_files) {close(pipe_fd[1]);}

    } else {
      printf("Fork failed!\n");
      return 1;
    }
  }

  int status;
  int pid_count=0;
  signal(SIGALRM, alarm_s);

 if(timeout>0)
 {
  while (active_child_processes > 0) {
    // your code here
    alarm(timeout);
    waitpid(ch_pid[pid_count], &status, WNOHANG);
    active_child_processes -= 1;
    pid_count++;
  }}
  else
  {
    while(active_child_processes >0){
     wait(&status);
     active_child_processes -= 1;
    }
   }

  struct MinMax *min_max=malloc(sizeof(struct MinMax));
  int min = INT_MAX;
  int max = INT_MIN;
  FILE *min_max_file;
  char f_path[10];
   
 
  for (int i = 0; i < pnum; i++) {
  
    if (with_files) {
      // read from files
	sprintf(f_path, "%d.bin",i);
	if((min_max_file = fopen(f_path, "rb"))==NULL)
	{printf("Error openning file");}
	if(fread(min_max, sizeof(struct MinMax),1, min_max_file) != 1)
	{ printf("Error reading from file"); return 1;}
	fclose(min_max_file);
	remove(f_path);
    } else {
      // read from pipes
	read(pipe_fd[0], min_max, sizeof(struct MinMax));
    }

    if (min > min_max[0].min) min = min_max[0].min;
    if (max < min_max[0].max) max = min_max[0].max;
    printf("-----");
  }

  struct timeval finish_time;
  gettimeofday(&finish_time, NULL);

  double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
  elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

  free(array);
  free(min_max);
 if(!with_files) {close(pipe_fd[0]);}

  printf("Min: %d\n", min);
  printf("Max: %d\n", max);
  printf("Elapsed time: %fms\n", elapsed_time);
  fflush(NULL);
  return 0;
}
