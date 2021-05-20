#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char **argv)
{
	int pid = fork();
	if ( pid == 0 ) 
		execv( "sequential_min_max", argv);
    else 
    {
    wait(NULL);
    printf( "Finished executing the parent process\n" );
    }
	return 0;
}