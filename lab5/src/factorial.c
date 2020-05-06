#define LOCK(a) pthread_mutex_lock(&a)
#define UNLOCK(a) pthread_mutex_unlock(&a)
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/time.h>
#include <pthread.h>
#include <getopt.h>

volatile size_t global_result = 1;
size_t global_mod = 1;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

struct fact_data
{
    unsigned int start;
    unsigned int end;
};

void thread_fact(void *args)
{
    struct fact_data data = *((struct fact_data *)args);
    size_t local_result = 1;
    for (size_t i = data.start; i < data.end; ++i)
    {
        local_result = local_result * (i % global_mod) % global_mod;
    }

    LOCK(mutex);
    global_result = global_result * (local_result) % global_mod;
    UNLOCK(mutex);
};

int main(int argc, char **argv)
{

    uint32_t threads_num = -1;
    uint32_t f_k = -1;
    uint32_t f_mod = -1;

    while (1)
    {
        int current_optind = optind ? optind : 1;

        static struct option options[] = {{"k", required_argument, 0, 0},
                                          {"mod", required_argument, 0, 0},
                                          {"t_num", required_argument, 0, 0},
                                          {0, 0, 0, 0}};

        int option_index = 0;
        int c = getopt_long(argc, argv, "f", options, &option_index);

        if (c == -1)
            break;

        switch (c)
        {
        case 0:
            switch (option_index)
            {
            case 0:
                f_k = atoi(optarg);
                break;
            case 1:
                f_mod = atoi(optarg);
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

    if (optind < argc)
    {
        printf("Has at least one no option argument\n");
        return 1;
    }

    if (f_k < 0 || f_mod < 1 || threads_num < 1)
    {
        printf("Usage: %s --k \"num\" --mod \"num\" --t_num \"num\"\n",
               argv[0]);
        return 1;
    }

    global_mod = f_mod;
    pthread_t threads[threads_num];

    int step_for_thread = threads_num < f_k
                             ? (f_k / threads_num)
                             : 1;

    struct timeval start_time;
    gettimeofday(&start_time, NULL);

    struct fact_data fact_args[threads_num];
    for(size_t i = 0; i < threads_num; ++i)
    {
        fact_args[i].start = i*step_for_thread + 1;
        fact_args[i].end = fact_args[i].start + step_for_thread;
        /*fact_args[i].end = fact_args[i].end > f_k + 1
                         ? fact_args[i].end
                         : f_k + 1;*/
        if(i == threads_num - 1)
            fact_args[i].end = f_k + 1;
        printf("%d: %d-%d\n", i, fact_args[i].start, fact_args[i].end);

        if (pthread_create(&threads[i], NULL, thread_fact, (void *)(&(fact_args[i]))))
        {
            printf("Error: pthread_create failed!\n");
            return 1;
        }
    }

    for (size_t i = 0; i < threads_num; i++)
    {
        pthread_join(threads[i], NULL);
    }

    struct timeval finish_time;
    gettimeofday(&finish_time, NULL);

    double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
    elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

    printf("Result: %lld\n", global_result);
    printf("Time: %f\n", elapsed_time);

    return 0;
}