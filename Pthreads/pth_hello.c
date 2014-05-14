#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#pragma comment(lib,"pthreadVC2.lib")

/* The number of threads */
int thread_count;

void* Hello(void* rank); /* Thread function */

int main(int argc,char* argv[])
{
	long thread;
	pthread_t* thread_handles;

	/* Get number of threads from command line */
	thread_count = strtol(argv[1],NULL,10);

	thread_handles = (pthread_t*)malloc(thread_count*sizeof(pthread_t));

	for(thread = 0;thread<thread_count;thread++)
		pthread_create(&thread_handles[thread],NULL,Hello,(void*)thread);

	printf("Hello from the main thread\n");

	for(thread = 0;thread<thread_count;thread++)
		pthread_join(thread_handles[thread],NULL);

	free(thread_handles);
	return 0;
}   /* main */

void* Hello(void* rank)
{
	long my_rank = (long) rank;
	           /* Use long in case of 64-bit system */

	printf("Hello from thread %1d of %d\n",my_rank,thread_count);
	return NULL;
}   /* Hello */