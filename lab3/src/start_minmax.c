#include <stdio.h>
#include <unistd.h>
int main(int argc, char **argv)
{
  printf("Starting the %s...\n\n", argv[0]);  
  
  int pid = fork();

  if ( pid == 0 ) {
  execv( "parallel_min_max", argv );
  }

  wait( 2 );

  printf("Finished executing %s\n", argv[0]);

  return 0;
}
