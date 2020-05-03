#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/time.h>
#include <pthread.h>
#include <getopt.h>

#include "utils.h"

struct SumArgs {
  int *array;
  int begin;
  int end;
};

int64_t Sum(const struct SumArgs *args) {
  int64_t sum = 0;
  for(int i = (*args).begin; i < (*args).end; ++i){
    sum += (*args).array[i];
  }
  return sum;
}

void *ThreadSum(void *args) {
  struct SumArgs *sum_args = (struct SumArgs *)args;
  return (void *)(size_t)Sum(sum_args);
}

int main(int argc, char **argv) {
  

  uint32_t threads_num = -1;
  uint32_t array_size = -1;
  uint32_t seed = -1;


  while (1) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"seed", required_argument, 0, 0},
                                      {"array_size", required_argument, 0, 0},
                                      {"t_num", required_argument, 0, 0},
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
            threads_num = atoi(optarg);
            break;

          defalut:
            printf("Index %d is out of options\n", option_index);
        }
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

  if (seed < 0 || array_size < 1 || threads_num < 1) {
    printf("Usage: %s --seed \"num\" --array_size \"num\" --t_num \"num\"\n",
           argv[0]);
    return 1;
  }



  pthread_t threads[threads_num];


  int *array = malloc(sizeof(int) * array_size);
  GenerateArray(array, array_size, seed);
  int step_for_array = threads_num < array_size 
                      ? (array_size / threads_num + 1)
                      : 1;
 
  struct timeval start_time;
  gettimeofday(&start_time, NULL);


  struct SumArgs args[threads_num];
  for (uint32_t i = 0; i < threads_num; i++) {
    args[i].array = array;
    args[i].begin = step_for_array*i;
    args[i].end = args[i].begin+step_for_array;
    args[i].end = args[i].end < array_size 
                  ? args[i].end
                  : array_size;
    //printf("%d: %d-%d\n", i, args[i].begin, args[i].end);

    if (pthread_create(&threads[i], NULL, ThreadSum, (void *)(&(args[i])))) {
      printf("Error: pthread_create failed!\n");
      return 1;
    }
  }

  int64_t total_sum = 0;
  for (size_t i = 0; i < threads_num; i++) {
    int64_t sum = 0;
    pthread_join(threads[i], (void **)&sum);
    total_sum += sum;
  }

  struct timeval finish_time;
  gettimeofday(&finish_time, NULL);

  double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
  elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

  free(array);
  printf("Total: %lld\n", total_sum);
  printf("Elapsed time: %f ms\n", elapsed_time);
  return 0;
}
