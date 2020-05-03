#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main ()
{
  pid_t child_pid;

  child_pid = fork ();
  if (child_pid > 0) {
    sleep (15);
  }
  else {
    printf("Becomes zombie\n");
    exit (0);
  }
	printf("Main process end\n");
  return 0;
}
