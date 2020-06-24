#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/file.h>

#include <getopt.h>

#include "find_min_max.h"
#include "utils.h"

pid_t* children;
int pnum;
void MyAlarm(){
    int status, i;
    for (i = 0; i < pnum; i++) {
        //Приостанавливает выполнение вызывающего процесса до тех пор, 
        //пока дочерний элемент, указанный в аргументе pid, не завершиться.
        //WNOHANG - означает немедленное возвращение управления, 
        //если ни один дочерний процесс не завершил выполнение.
        int result = waitpid(children[i], &status, WNOHANG);
        if (result == 0) {
            //Немедленное завершение процесса
            kill(children[i], SIGKILL);
            printf("%d is killed\n", children[i]);
        } 
        else {
            printf("%d is finished\n", children[i]);
        }
    }
}

int main(int argc, char **argv) {
   // Инициализация до начала работы
  int seed = -1;
  int array_size = -1;
  pnum = -1;
  bool with_files = false;
  int timeout = -1;
  int f;

  while (true) {
    int current_optind = optind ? optind : 1;
    // Структура option для getopt_long()
    // Имя опции без двойного дефиса, имеет ли эта опция аргумент, flag=0 для val=getopt_long()
    static struct option options[] = {{"seed", required_argument, 0, 0},
                                      {"array_size", required_argument, 0, 0},
                                      {"pnum", required_argument, 0, 0},
                                      {"by_files", required_argument, 0, 'f'},
                                      {"timeout", required_argument, 0, 0},
                                      {0, 0, 0, 0}};
    int option_index = 0;
    // Возвращает первый параметр и задает некоторые глобальные переменные
    int c = getopt_long(argc, argv, "f", options, &option_index);
    // Если не будет найдено опций, то функция вернет -1
    if (c == -1) break;
    // Как только getopt() распознает параметр, конструкция switch определит, 
    // какой точно параметр был найден, и отметит соответствующее поле в структуре
    switch (c) {
      case 0:
        switch (option_index) {
          case 0:
            seed = atoi(optarg);
            // your code here
            // error handling
            if (seed <= 0) {
                printf("seed is a positive number\n");
                return -1;
            }
            /////////////////
            break;
          case 1:
            array_size = atoi(optarg);
            // your code here
            // error handling
            if (array_size <= 0) {
                printf("array_size is a positive number\n");
                return -1;
            }
            /////////////////
            break;
          case 2:
            pnum = atoi(optarg);
            // your code here
            // error handling
            if (pnum <= 0) {
                printf("pnum is a positive number\n");
                return -1;
            }
            /////////////////
            break;
          case 3:
            f = atoi(optarg);
            if ((f < 0)||(f > 1)) {
                printf("by_files is a positive number\n");
                return -1;
            }
            if (f) 
                with_files = true;
            break;
          case 4:
            timeout = atoi(optarg);
            // your code here
            // error handling
            if (timeout <= 0) {
                printf("timeout is a positive number\n");
                return 1;
            }
            /////////////////
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
    //Так писать в строке аргументов: --seed 10 --array_size 10 --pnum 10
    printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\" \n",
           argv[0]);
    return 1;
  }

  int *array = malloc(sizeof(int) * array_size);
  GenerateArray(array, array_size, seed);
  int active_child_processes = 0;

  // Время начала
  struct timeval start_time;
  gettimeofday(&start_time, NULL);

  // Файл
  FILE *fp;
  char name[] = "par_min_max.txt";
  if ((fp = fopen(name, "w")) == NULL) {
    printf("Failed to create file");
    getchar();
    return -1;
  }
  int file = fileno(fp);

  // Pipe
  // Данные обрабатываются по алгоритму "первым пришел, первым обслужен"
  int file_descriptor[pnum][2];
  int i;
  for (i = 0; i < pnum; i++) {
    if (pipe(file_descriptor[pnum]) == -1)
    {
        return -1;
    }
  }
  children = malloc(sizeof(pid_t)*pnum);

  for (i = 0; i < pnum; i++) {
    printf("%d\n", i);
    // child_pid - идентификатор процесса
    pid_t child_pid = fork();
    // Выполняется уже двумя процессами одновременно
    if (child_pid >= 0) {
      // successful fork
      printf("Fork: %d\n", child_pid);
      children[i] = child_pid;
      active_child_processes += 1;
      // Процесс-родитель получает идентификатор (PID) потомка
      // Процесс-потомок получает в качестве кода возврата значение 0
      if (child_pid == 0) {
        // child process
        printf("Child: %d\n", child_pid);
        // parallel somehow
        struct MinMax min_max1 = GetMinMax(array, 0, array_size);
        char bmin[150];
        sprintf(bmin, "%d\n", min_max1.min);
        char bmax[150];
        sprintf(bmax, "%d\n", min_max1.max);

        if (with_files) {
          // use files here 2
          // Установить эксклюзивную блокировку
          flock(file, LOCK_EX);
          write(file, (void*)bmin, strlen(bmin));
          write(file, (void*)bmax, strlen(bmax));
          flock(file, LOCK_UN);
          fclose(fp);
        } else {
          // use pipe here 3
          write(file_descriptor[i][1], (void*)bmin, strlen(bmin));
          write(file_descriptor[i][1], (void*)bmax, strlen(bmax));
        }
        return 0;
      }

    } else {
      printf("Fork failed!\n");
      return 1;
    }
  }

  if (timeout != -1) {
    //Привязка функции MyAlarm к сигналу SIGALRM
    signal(SIGALRM, MyAlarm);
    //Установка будильника для запуска MyAlarm
    alarm(timeout);
    pause();
  } 
  else {
        while (active_child_processes > 0) {
            // your code here
            wait(0);
            active_child_processes -= 1;
        }
  }

  struct MinMax min_max;
  min_max.min = INT_MAX;
  min_max.max = INT_MIN;

  for (i = 0; i < pnum; i++) {
    int min = INT_MAX;
    int max = INT_MIN;
    char buffer[150];

    if (with_files) {
      // read from files 2 
      FILE *fp;
      if ((fp = fopen(name, "r")) == NULL) {
        printf("Failed to create file");
        getchar();
        return -1;
      }
      fscanf(fp,"%s",buffer);
      min_max.min = atoi(buffer);
      fscanf(fp,"%s",buffer);
      min_max.max = atoi(buffer);
      fclose(fp);
    } else {
      // read from pipes 3
      read(file_descriptor[pnum][0],buffer,10);
      min_max.min = atoi(buffer);
      read(file_descriptor[pnum][0],buffer,10);
      min_max.max = atoi(buffer);
    }

    if (min < min_max.min) min_max.min = min;
    if (max > min_max.max) min_max.max = max;
  }

  // Время конца
  struct timeval finish_time;
  gettimeofday(&finish_time, NULL);

  // Промежуток
  double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
  //elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

  free(array);

  printf("Min: %d\n", min_max.min);
  printf("Max: %d\n", min_max.max);
  printf("Elapsed time: %fms\n", elapsed_time);
  fflush(NULL);
  return 0;
}
