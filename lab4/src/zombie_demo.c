#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main ()
{
  pid_t child_pid;

  child_pid = fork ();
  if (child_pid > 0) {
    // if in parent process
    sleep (15);
  }
  else {
    //if in fork
    printf("Becomes zombie\n");
    exit (0);
  }
	printf("Main process end\n");
  int status = 0;
  wait(&status);
  return 0;
}
