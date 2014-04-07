#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


//establish function for thread
void* printThing(void*);

int main ()
{
	pthread_t thread1;
	int waitTime = 3000000;

	pthread_create(&thread1, NULL, printThing, (void*)waitTime);

	pthread_join(thread1, NULL);

	return 0;
}

void* printThing(void* wait)
{
	int waitTime = (int) wait;
	usleep(waitTime);
	printf("%d \n", waitTime);
	pthread_exit(0);
}
