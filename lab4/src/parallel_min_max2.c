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

/////////////////////
volatile pid_t* children;
volatile int child_cnt;


  void timeout_alarm (int var){
  
    printf("Process terminated by timer\n");
    for (int i = 0; i < child_cnt; i++){
    kill(children[i], SIGKILL);
    }
  }
/////////////////////////

int main(int argc, char **argv) {
  int seed = -1;
  int array_size = -1;
  int pnum = -1;
  int timeout = 0;
  bool with_files = false;

  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"seed", required_argument, 0, 0},
                                      {"array_size", required_argument, 0, 0},
                                      {"pnum", required_argument, 0, 0},
                                      {"timeout", required_argument, 0, 0},
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
            break;
          case 1:
            array_size = atoi(optarg);
            break;
          case 2:
            pnum = atoi(optarg);
            break;
          case 3:
            timeout = atoi(optarg);
            break;
          case 4:
            with_files = true;
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
    printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\" [--by_files] [--timeout \"num\"]\n",
           argv[0]);
    return 1;
  }

  int *array = malloc(sizeof(int) * array_size);
  GenerateArray(array, array_size, seed);
  int active_child_processes = 0;
  int proc_array_len = (pnum < array_size ? (array_size/pnum): 1); // array len for one process

  struct timeval start_time;
  gettimeofday(&start_time, NULL);
  
  FILE *file;
  int pipefd[2];
  if (with_files)
  {
    file = fopen("sync_file", "wb+");
  }
  else
  if (pipe(pipefd) == -1)
  {
    perror("pipe error");
    exit(EXIT_FAILURE);
  }
  
  /////////////////
  child_cnt = pnum;
  children = malloc(pnum*sizeof(pid_t));
  ////////////////////
  
  
  for (int i = 0; i < pnum; i++) {
    pid_t child_pid = fork();
    if (child_pid >= 0) {
      // successful fork
      active_child_processes += 1;
      //printf("Created fork: %d\n", child_pid);
      if (child_pid == 0) {
        // child process


        

        unsigned int a_l = proc_array_len * (active_child_processes - 1);
        unsigned int a_r = a_l + proc_array_len;
        
        a_l = a_l > array_size ? array_size : a_l;
        a_r = a_r > array_size ? array_size : a_r;
        
        if (active_child_processes == pnum) a_r = array_size;
        struct MinMax min_max = GetMinMax(array, a_l, a_r);

        if (with_files) 
        {
          fwrite(&min_max, sizeof(struct MinMax), 1, file);
        } 
        else 
        {
          write(pipefd[1], &min_max, sizeof(struct MinMax));
        }
        return 0;
      }
      else
      {
        //////////
      children[i] = child_pid;
      }

    } else {
      printf("Fork failed!\n");
      return 1;
    }
  }

  
  ///////////////////////////////////////
        if (timeout > 0)
        {
        signal(SIGALRM, timeout_alarm);
        alarm(timeout);
        }

  int status = WNOHANG;
  while (active_child_processes > 0) {
    
    
    if(wait(&status) > 0)
      active_child_processes -= 1;
        
  }////////////////////////////////////
  
  if (with_files)
  {
    fclose(file);
    file = fopen("sync_file", "rb");
  }

  struct MinMax min_max;
  min_max.min = INT_MAX;
  min_max.max = INT_MIN;

  for (int i = 0; i < pnum; i++) {
    int min = INT_MAX;
    int max = INT_MIN;
    struct MinMax tmp;

    if (with_files) 
    {
      fread(&tmp, sizeof(struct MinMax), 1, file);
    } 
    else 
    {
      read(pipefd[0], &tmp, sizeof(struct MinMax));
    }
    min = tmp.min;
    max = tmp.max;

    if (min < min_max.min) min_max.min = min;
    if (max > min_max.max) min_max.max = max;
  }

  struct timeval finish_time;
  gettimeofday(&finish_time, NULL);

  double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
  elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

  free(array);
  free(children);

  printf("Min: %d\n", min_max.min);
  printf("Max: %d\n", min_max.max);
  printf("Elapsed time: %fms\n", elapsed_time);
  fflush(NULL);
  return 0;
}
