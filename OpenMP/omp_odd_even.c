/* File:    odd_even.c
 *
 * Purpose: Use odd-even transposition sort to sort a list of ints.
 *
 * Compile: gcc -g -Wall -o odd_even odd_even.c
 * Run:     odd_even <n> <g|i> <c>
 *             n:   number of elements in list
 *            'g':  generate list using a random number generator
 *            'i':  user input list
 *            'c':  number of threads
 *
 * Input:   list (optional)
 * Output:  sorted list
 *
 * IPP:     Section 3.7.1 (p. 128) and Section 5.6.2 (pp. 233 and ff.)
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>


/* Keys in the random list in the range 0 <= key < RMAX */
const int RMAX = 100000;

void Usage(char* prog_name);
void Get_args(int argc, char* argv[], int* n_p, char* g_i_p,int* thread_count);
void Generate_list(int a[], int n);
void Print_list(int a[], int n, char* title);
void Read_list(int a[], int n);
void Omp_odd_even_sort(int a[], int n,int thread_count);

/*-----------------------------------------------------------------*/
int main(int argc, char* argv[]) {
   int  n;
   char g_i;
   int* a;
   int thread_count;
   double beg,end;

   Get_args(argc, argv, &n, &g_i,&thread_count);
   a = (int*) malloc(n*sizeof(int));
   if (g_i == 'g') {
      Generate_list(a, n);
      Print_list(a, n, "Before sort");
   } else {
      Read_list(a, n);
   }

   beg = omp_get_wtime();
   Omp_odd_even_sort(a, n,thread_count);
   end = omp_get_wtime();

   Print_list(a, n, "After sort");
   
   printf("Time %f\n",end-beg);

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
   fprintf(stderr, "  'c':  number of count\n");
}  /* Usage */


/*-----------------------------------------------------------------
 * Function:  Get_args
 * Purpose:   Get and check command line arguments
 * In args:   argc, argv
 * Out args:  n_p, g_i_p, thread_count
 */
void Get_args(int argc, char* argv[], int* n_p, char* g_i_p,int* thread_count) {
   if (argc != 4 ) {
      Usage(argv[0]);
      exit(0);
   }
   *n_p = atoi(argv[1]);
   *g_i_p = argv[2][0];
   *thread_count = strtol(argv[3],NULL,10);

   if (*n_p <= 0 
	   || (*g_i_p != 'g' && *g_i_p != 'i')
	   || *thread_count<1) {
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

   srand((unsigned)time(NULL));
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
      scanf_s("%d", &a[i]);
}  /* Read_list */


/*-----------------------------------------------------------------
 * Function:     Odd_even_sort
 * Purpose:      Sort list using odd-even transposition sort
 * In args:      n
 * In/out args:  a
 */
void Omp_odd_even_sort(
      int  a[]          /* in/out */, 
      int  n            /* in     */,
      int thread_count  /* in     */) {
   int phase, i, temp;

# pragma omp parallel num_threads(thread_count) \
	default(none) shared(a, n) private(i,temp,phase)
   for (phase = 0; phase < n; phase++) 
      if (phase % 2 == 0) { /* Even phase */
# pragma omp for
         for (i = 1; i < n; i += 2) 
            if (a[i-1] > a[i]) {
               temp = a[i];
               a[i] = a[i-1];
               a[i-1] = temp;
            }
      } else { /* Odd phase */
# pragma omp for
         for (i = 1; i < n-1; i += 2)
            if (a[i] > a[i+1]) {
               temp = a[i];
               a[i] = a[i+1];
               a[i+1] = temp;
            }
      }
}  /* Odd_even_sort */
