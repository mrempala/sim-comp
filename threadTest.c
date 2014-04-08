// might have some extra includes in here
// not sure if usleep is declared in stdlib
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

/*	***NOTE*** 
	must include -pthread when compiling in makefile
*/

// establish function for thread
// must be void* I believe
void* printThing(void*);
 
// global variable to monitor interrupt
int interrupted = 0;

int main ()
{
	pthread_t thread1, thread2;
	int waitTime = 3000000;

	pthread_create(&thread1, NULL, printThing, (void*)waitTime);
	pthread_create(&thread2, NULL, printThing, (void*)waitTime);

	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);
	
	//check for an interrupt
/*	while (threads are still running)
		if (interrupted == 1)
		{
			//handle interrupt
			interrupted = 0;
		}
*/
	return 0;
}

void* printThing(void* wait)
{
	int waitTime = (int) wait;
	usleep(waitTime);
	//wait for interrupt to stop being serviced
	while (interrupted == 1);
	
	//set their own interrupt
	interrupted = 1;
	printf("%d \n", waitTime);	

	//at the moment handling their own interrupt
	interrupted = 0;

	pthread_exit(0);
}
