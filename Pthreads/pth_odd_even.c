/* File:    odd_even.c
 *
 * Purpose: Use odd-even transposition sort to sort a list of ints.
 *
 * Compile: gcc -g -Wall -o odd_even odd_even.c
 * Run:     odd_even <n> <g|i> <c>
 *             n:   number of elements in list
 *            'g':  generate list using a random number generator
 *            'i':  user input list
 *            'c':  the number of threads
 *
 * Input:   list (optional)
 * Output:  sorted list
 *
 * IPP:     Section 3.7.1 (p. 128) and Section 5.6.2 (pp. 233 and ff.)
 */
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <Windows.h>

#pragma comment(lib,"pthreadVC2.lib")

/* Keys in the random list in the range 0 <= key < RMAX */
const int RMAX = 1000;

int thread_count;
int* a;
int n;
int phase;
int counter;
pthread_mutex_t mutex;
pthread_cond_t cond_var;

void Usage(char* prog_name);
void Get_args(int argc, char* argv[], int* n_p, char* g_i_p,int*thread_count);
void Generate_list(int a[], int n);
void Print_list(int a[], int n, char* title);
void Read_list(int a[], int n);
void* Odd_even_sort(void* rank);

/*-----------------------------------------------------------------*/
int main(int argc, char* argv[]) {
   char g_i;
   int i;
   pthread_t* thread_handles = NULL;
   double beg,end;

   Get_args(argc, argv, &n, &g_i,&thread_count);
   a = (int*) malloc(n*sizeof(int));
   if (g_i == 'g') {
      Generate_list(a, n);
      Print_list(a, n, "Before sort");
   } else {
      Read_list(a, n);
   }

  
   phase = 0;
   counter=0;
   pthread_cond_init(&cond_var,NULL);
   pthread_mutex_init(&mutex,NULL);
   thread_handles = (pthread_t*)malloc(thread_count*sizeof(pthread_t));

   beg = GetTickCount();
   for(i=0;i<thread_count;i++)
	   pthread_create(&thread_handles[i],NULL,Odd_even_sort,(void*)i);

   for(i=0;i<thread_count;i++)
	   pthread_join(thread_handles[i],NULL);
   end = GetTickCount();

   pthread_cond_destroy(&cond_var);
   pthread_mutex_destroy(&mutex);
   Print_list(a, n, "After sort");
   printf("\nTime: %fs\n",(end-beg)/1000);
   
   free(a);
   return 0;
}  /* main */


/*-----------------------------------------------------------------
 * Function:  Usage
 * Purpose:   Summary of how to run program
 */
void Usage(char* prog_name) {
   fprintf(stderr, "usage:   %s <n> <g|i> <c>\n", prog_name);
   fprintf(stderr, "   n:   number of elements in list\n");
   fprintf(stderr, "  'g':  generate list using a random number generator\n");
   fprintf(stderr, "  'i':  user input list\n");
   fprintf(stderr, "  'c':  number of threads\n");
}  /* Usage */


/*-----------------------------------------------------------------
 * Function:  Get_args
 * Purpose:   Get and check command line arguments
 * In args:   argc, argv
 * Out args:  n_p, g_i_p, thread_count
 */
void Get_args(int argc, char* argv[], int* n_p, char* g_i_p,int*thread_count) {
   if (argc != 4 ) {
      Usage(argv[0]);
      exit(0);
   }
   *n_p = atoi(argv[1]);
   *g_i_p = argv[2][0];
   *thread_count = strtol(argv[3],NULL,10);

   if (*n_p <= 0 || (*g_i_p != 'g' && *g_i_p != 'i') ) {
      Usage(argv[0]);
      exit(0);
   }
}  /* Get_args */


/*-----------------------------------------------------------------
 * Function:  Generate_list
 * Purpose:   Use random number generator to generate list elements
 * In args:   n
 * Out args:  a
 */
void Generate_list(int a[], int n) {
   int i;

   srand(0);
   for (i = 0; i < n; i++)
      a[i] = rand() % RMAX;
}  /* Generate_list */


/*-----------------------------------------------------------------
 * Function:  Print_list
 * Purpose:   Print the elements in the list
 * In args:   a, n
 */
void Print_list(int a[], int n, char* title) {
   int i;

   printf("%s:\n", title);
   for (i = 0; i < n; i++)
      printf("%d ", a[i]);
   printf("\n\n");
}  /* Print_list */


/*-----------------------------------------------------------------
 * Function:  Read_list
 * Purpose:   Read elements of list from stdin
 * In args:   n
 * Out args:  a
 */
void Read_list(int a[], int n) {
   int i;

   printf("Please enter the elements of the list\n");
   for (i = 0; i < n; i++)
      scanf("%d", &a[i]);
}  /* Read_list */


/*-----------------------------------------------------------------
 * Function:     Odd_even_sort
 * Purpose:      Sort list using odd-even transposition sort
 * In args:      n
 * In/out args:  a
 */
void* Odd_even_sort(void* rank) {
	long my_rank = (long)rank;
	int local_a = my_rank*n/thread_count;
	int local_b = local_a+n/thread_count;
	int i, temp;
	
	//每一个线程的i起始点均为奇数
	if(local_a%2==0)
		   local_a++;

   while(phase<n)
   {
	   i = local_a;

	   //防止除不尽时，最后一个线程被分配的数组被遗漏或越界
	   if(my_rank==thread_count-1)
		   if(phase%2==0)
			   local_b=n;
		   else
			   local_b=n-1;
      if (phase % 2 == 0) { /* Even phase */
		  //当末端为奇数时仍要计算
		  if(local_b<n&&local_b%2)
			  local_b++;
         for (; i < local_b; i += 2) 
            if (a[i-1] > a[i]) {
               temp = a[i];
               a[i] = a[i-1];
               a[i-1] = temp;
            }
      } else { /* Odd phase */
		  if(local_b<n-1&&local_b%2)
			  local_b++;
         for (; i < local_b; i += 2)
            if (a[i] > a[i+1]) {
               temp = a[i];
               a[i] = a[i+1];
               a[i+1] = temp;
            }
      }

	  //路障，线程应同时开始奇迭代或偶迭代
	  /* Barrier */
	  pthread_mutex_lock(&mutex);
	  counter++;
	  if(counter == thread_count){
		  counter = 0;
		  phase++;
		  pthread_cond_broadcast(&cond_var);
	  }else{
		  while(pthread_cond_wait(&cond_var,&mutex)!=0);
	  }
	  pthread_mutex_unlock(&mutex);
   }
	  return NULL;
}  /* Odd_even_sort */

