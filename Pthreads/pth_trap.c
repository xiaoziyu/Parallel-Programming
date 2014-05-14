/* File:    trap.c
 * Purpose: Calculate definite integral using trapezoidal 
 *          rule.
 *
 * Input:   a, b, n
 * Output:  Estimate of integral from a to b of f(x)
 *          using n trapezoids.
 *
 * Compile: gcc -g -Wall -o trap trap.c
 * Usage:   ./trap n(n is the counts of the threads)
 *
 * Note:    The function f(x) is hardwired.
 *
 * IPP:     Section 3.2.1 (pp. 94 and ff.) and 5.2 (p. 216)
 */
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <Windows.h>

#pragma comment(lib,"pthreadVC2.lib")

int thread_count;
double a,b;         /* Left and right endpoints   */
int     n;          /* Number of trapezoids       */
double h;           /* Height of trapezoids       */
double  sum;        /* Store result in sum   */
pthread_mutex_t sum_mutex;              /* mutex of sum */

double f(double x);    /* Function we're integrating */
void* Trap(void* rank);

int main(int argc,char* argv[]) {
   int i;
   pthread_t* thread_handles = NULL;
   double beg,end;

   thread_count = strtol(argv[1],NULL,10);

   printf("Enter a, b, and n\n");
   scanf("%lf", &a);
   scanf("%lf", &b);
   scanf("%d", &n);

   h = (b-a)/n;
   sum = 0;
   thread_handles = (pthread_t*)malloc(thread_count*sizeof(pthread_t));
   pthread_mutex_init(&sum_mutex,NULL);

   beg = GetTickCount();
   for(i=0;i<thread_count;i++)
	   pthread_create(&thread_handles[i],NULL,Trap,(void*)i);

   for(i=0;i<thread_count;i++)
	   pthread_join(thread_handles[i],NULL);
   end = GetTickCount();
   
   pthread_mutex_destroy(&sum_mutex);
   printf("With n = %d trapezoids, our estimate\n", n);
   printf("of the integral from %f to %f = %.15f\n",
      a, b, sum);
   printf("\nTime: %fs\n",(end-beg)/1000);

   return 0;
}  /* main */

/*------------------------------------------------------------------
 * Function:    Trap
 * Purpose:     Estimate integral from a to b of f using trap rule and
 *              n trapezoids
 * Input args:  a, b, n, h
 * Return val:  Estimate of the integral 
 */
void* Trap(void* rank) {
	long my_rank = (long)rank;
	double local_n = n/thread_count;
	double local_a = a+my_rank*local_n*h;
	double local_b = local_a + local_n*h;
	
	if(my_rank==rank-1)
		local_b = n;

	double integral;
	int k;

	integral = (f(local_a) + f(local_b))/2.0;
	for (k = 1; k <= local_n-1; k++) {
		integral += f(local_a+k*h);
	}
	integral = integral*h;

	pthread_mutex_lock(&sum_mutex);
	sum+=integral;
	pthread_mutex_unlock(&sum_mutex);

	return NULL;
}  /* Trap */

/*------------------------------------------------------------------
 * Function:    f
 * Purpose:     Compute value of function to be integrated
 * Input args:  x
 */
double f(double x) {
   double return_val;

   return_val = x*x;
   return return_val;
}  /* f */
