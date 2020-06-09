#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <errno.h>
#include <getopt.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>

#include "mult_modul.h"

volatile uint64_t global_result = 1;
volatile uint64_t global_mod = 1;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

struct Server
{
  char ip[255];
  int port;
};

struct thr_params
{
  int start;
  int end;
  struct Server srv;
};

void srv_connection(void *args)
{
  struct thr_params params = *((struct thr_params *)args);

  //MAGIC:
  struct hostent *hostname = gethostbyname(params.srv.ip);
  if (hostname == NULL)
  {
    fprintf(stderr, "gethostbyname failed with %s\n", params.srv.ip);
    exit(1);
  }

  struct sockaddr_in server;
  server.sin_family = AF_INET;
  server.sin_port = htons(params.srv.port);
  server.sin_addr.s_addr = *((unsigned long *)hostname->h_addr);

  int sck = socket(AF_INET, SOCK_STREAM, 0);
  if (sck < 0)
  {
    fprintf(stderr, "Socket creation failed!\n");
    exit(1);
  }

  if (connect(sck, (struct sockaddr *)&server, sizeof(server)) < 0)
  {
    fprintf(stderr, "Connection failed\n");
    exit(1);
  }

  uint64_t begin = params.start;
  uint64_t end = params.end;

  char task[sizeof(uint64_t) * 3];
  memcpy(task, &begin, sizeof(uint64_t));
  memcpy(task + sizeof(uint64_t), &end, sizeof(uint64_t));
  memcpy(task + 2 * sizeof(uint64_t), &global_mod, sizeof(uint64_t));

  if (send(sck, task, sizeof(task), 0) < 0)
  {
    fprintf(stderr, "Send failed\n");
    exit(1);
  }

  char response[sizeof(uint64_t)];
  if (recv(sck, response, sizeof(response), 0) < 0)
  {
    fprintf(stderr, "Recieve failed\n");
    exit(1);
  }

  uint64_t answer = 0;
  memcpy(&answer, response, sizeof(uint64_t));
  close(sck);
  printf("Got answer: %lld\n", answer);
  pthread_mutex_lock(&mutex);
  global_result = MultModulo(global_result, answer, global_mod);
  pthread_mutex_unlock(&mutex);
}

bool ConvertStringToUI64(const char *str, uint64_t *val)
{
  char *end = NULL;
  unsigned long long i = strtoull(str, &end, 10);
  if (errno == ERANGE)
  {
    fprintf(stderr, "Out of uint64_t range: %s\n", str);
    return false;
  }

  if (errno != 0)
    return false;

  *val = i;
  return true;
}

int main(int argc, char **argv)
{
  uint64_t k = -1;
  uint64_t mod = -1;
  char servers[255] = {'\0'}; // TODO: explain why 255

  while (true)
  {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"k", required_argument, 0, 0},
                                      {"mod", required_argument, 0, 0},
                                      {"servers", required_argument, 0, 0},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "", options, &option_index);

    if (c == -1)
      break;

    switch (c)
    {
    case 0:
    {
      switch (option_index)
      {
      case 0:
        ConvertStringToUI64(optarg, &k);
        // TODO: your code here
        break;
      case 1:
        ConvertStringToUI64(optarg, &mod);
        // TODO: your code here
        break;
      case 2:
        // TODO: your code here
        memcpy(servers, optarg, strlen(optarg));
        break;
      default:
        printf("Index %d is out of options\n", option_index);
      }
    }
    break;

    case '?':
      printf("Arguments error\n");
      break;
    default:
      fprintf(stderr, "getopt returned character code 0%o?\n", c);
    }
  }

  if (k < 1 || mod < 1 || !strlen(servers))
  {
    fprintf(stderr, "Using: %s --k 1000 --mod 5 --servers /path/to/file\n\tNote: max 256 servers.\n",
            argv[0]);
    return 1;
  }

  FILE *file = fopen(servers, "rt");
  if (file == NULL)
  {
    fprintf("Cannot open file %s", servers);
    return 1;
  }

  

  struct Server* servers_arr = malloc(256*sizeof(struct Server));
  unsigned int servers_num = 0;
  char str[32];
  while (servers_num < 256 && !feof(file))
  {
    fscanf(file, "%s\n", &str);
    char *marker = strtok(str, ":");
    memcpy(servers_arr[servers_num].ip, str, sizeof(str));
    servers_arr[servers_num].port = atoi(strtok(NULL, ":"));
    ++servers_num;
  }
  if (servers_num < 1)
  {
    fprintf("File %s contains no addresses.", servers);
    return 1;
  }

  pthread_t threads[servers_num];
  struct thr_params params_arr[servers_num];
  uint64_t step = k / servers_num;
  global_mod = mod;
  for (int i = 0; i < servers_num; i++)
  {
    params_arr[i].start = 1 + i * step;
    params_arr[i].end = 1 + (i + 1) * step;
    if (params_arr[i].end > k + 1)
      params_arr[i].end = k + 1;
    params_arr[i].srv = servers_arr[i];
    if (pthread_create(&threads[i], NULL, srv_connection, (void *)(&(params_arr[i]))))
    {
      printf("Error: pthread_create failed!\n");
      return 1;
    }
  }
  for (size_t i = 0; i < servers_num; i++)
  {
    pthread_join(threads[i], NULL);
  }
  printf("Calculation completed!\nResult: %lld\n", global_result);

  free(servers_arr);
  return 0;
}
