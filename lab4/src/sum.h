#include <stdint.h>


struct SumArgs {
  int *array;
  int begin;
  int end;
};

int64_t Sum(const struct SumArgs *args);