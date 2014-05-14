/* File:     mat_vect_mult.c
 *
 * Purpose:  Implement serial matrix-vector multiplication using
 *           one-dimensional arrays to store the vectors and the
 *           matrix.
 *
 * Compile:  gcc -g -Wall -o mat_vect_mult mat_vect_mult.c
 * Run:      ./mat_vect_mult
 *
 * Input:    Dimensions of the matrix (m = number of rows, n
 *              = number of columns)
 *           n-dimensional vector x
 * Output:   Product vector y = Ax
 *
 * Errors:   if the number of user-input rows or column isn't
 *           positive, the program prints a message and quits.
 * Note:     Define DEBUG for verbose output
 *
 * IPP:      Section 3.4.9 (pp. 113 and ff.), Section 4.3 (pp. 159
 *           and ff.), and Section 5.9 (pp. 252 and ff.)
 */
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <Windows.h>

#pragma comment(lib, "pthreadVC2.lib")

/* 0<=key<100 */
#define MAX 100

/* Global variable: accessible to all threads */
int thread_count;

double* A = NULL;
double* x = NULL;
double* y = NULL;
int m, n;

void Get_dims(int* m_p, int* n_p);
void Read_matrix(char prompt[], double A[], int m, int n);
void Read_vector(char prompt [], double x[], int n);
void Print_matrix(char title[], double A[], int m, int n);
void Print_vector(char title[], double y[], int m);
void* Mat_vect_mult(void* rank);
void Gen_num(double tar[],int N);

/*-------------------------------------------------------------------*/
int main(int argc,char* argv[]) {
   int i;
   double beg,end;
   pthread_t* thread_handles = NULL;
   
   /* Get number of threads from command line */
   thread_count = strtol(argv[1],NULL,10);

   thread_handles = (pthread_t*)malloc(thread_count*sizeof(pthread_t));

   Get_dims(&m, &n);
   A = (double*)malloc(m*n*sizeof(double));
   x = (double*)malloc(n*sizeof(double));
   y = (double*)malloc(m*sizeof(double));
   if (A == NULL || x == NULL || y == NULL) {
      fprintf(stderr, "Can't allocate storage\n");
      exit(-1);
   }
   Read_matrix("A", A, m, n);

#  ifdef DEBUG
   Print_matrix("A", A, m, n);
#  endif
   Read_vector("x", x, n);
#  ifdef DEBUG
   Print_vector("x", x, n);
#  endif

   beg = GetTickCount();
   for(i=0;i<thread_count;i++)
	   pthread_create(&thread_handles[i],NULL,Mat_vect_mult,(void*)i);

   for(i=0;i<thread_count;i++)
	   pthread_join(thread_handles[i],NULL);
   end = GetTickCount();


   Print_vector("y", y, m);
   printf("\nTime: %fs\n",(end-beg)/1000);

   free(A);
   free(x);
   free(y);
   return 0;
}  /* main */


/*-------------------------------------------------------------------
 * Function:   Get_dims
 * Purpose:    Read the dimensions of the matrix from stdin
 * Out args:   m_p:  number of rows
 *             n_p:  number of cols
 *
 * Errors:     If one of the dimensions isn't positive, the program
 *             prints an error and quits
 */
void Get_dims(
              int*  m_p  /* out */, 
              int*  n_p  /* out */) {
   printf("Enter the number of rows\n");
   scanf("%d", m_p);
   printf("Enter the number of columns\n");
   scanf("%d", n_p);

   if (*m_p <= 0 || *n_p <= 0) {
      fprintf(stderr, "m and n must be positive\n");
      exit(-1);
   }
}  /* Get_dims */

/*-------------------------------------------------------------------
 * Function:   Read_matrix
 * Purpose:    Read the contents of the matrix from stdin
 * In args:    prompt:  description of matrix
 *             m:       number of rows
 *             n:       number of cols
 * Out arg:    A:       the matrix
 */
void Read_matrix(
                 char    prompt[]  /* in  */, 
                 double  A[]       /* out */, 
                 int     m         /* in  */, 
                 int     n         /* in  */) {
//   int i, j;

//   printf("Enter the matrix %s\n", prompt);
//   for (i = 0; i < m; i++)
//      for (j = 0; j < n; j++)
//         scanf("%lf", &A[i*n+j]);

   /* generate numbers randomly */
   Gen_num(A,m*n);
}  /* Read_matrix */

/*-------------------------------------------------------------------
 * Function:   Read_matrix
 * Purpose:    Read a vector from stdin
 * In args:    prompt:  description of matrix
 *             n:       order of matrix
 * Out arg:    x:       the vector being read in
 */
void Read_vector(
                 char    prompt[]  /* in  */, 
                 double  x[]       /* out */, 
                 int     n         /* in  */) {
//   int i;

//   printf("Enter the vector %s\n", prompt);
//   for (i = 0; i < n; i++)
//      scanf("%lf", &x[i]);

	/* generate numbers randomly */
	Gen_num(x,n);
}  /* Read_vector */


/*-------------------------------------------------------------------
 * Function:   Print_matrix
 * Purpose:    Print a matrix to stdout
 * In args:    title:  title for output
 *             A:      the matrix
 *             m:      number of rows
 *             n:      number of cols
 */
void Print_matrix(
                  char    title[]  /* in */,
                  double  A[]      /* in */, 
                  int     m        /* in */, 
                  int     n        /* in */) {
   int i, j;

   printf("\nThe matrix %s\n", title);
   for (i = 0; i < m; i++) {
      for (j = 0; j < n; j++)
         printf("%f ", A[i*n+j]);
      printf("\n");
   }
}  /* Print_matrix */

/*-------------------------------------------------------------------
 * Function:   Print_vector
 * Purpose:    Print the contents of a vector to stdout
 * In args:    title:  title for output
 *             y:      the vector to be printed
 *             m:      the number of elements in the vector
 */
void Print_vector(
                  char    title[]  /* in */, 
                  double  y[]      /* in */, 
                  int     m        /* in */) {
   int i;

   printf("\nThe vector %s\n", title);
   for (i = 0; i < m; i++)
      printf("%f ", y[i]);
   printf("\n");
}  /* Print_vector */


/*-------------------------------------------------------------------
 * Function:   Mat_vect_mult
 * Purpose:    Multiply a matrix by a vector
 * In args:    A: the matrix
 *             x: the vector being multiplied by A
 *             m: the number of rows in A and components in y
 *             n: the number of columns in A components in x
 * Out args:   y: the product vector Ax
 */
void* Mat_vect_mult(void* rank) {
   long my_rank = (long)rank;
   int i, j;
   int local_m = m/thread_count;
   int my_first_row = my_rank*local_m;
   int my_last_row = (my_rank+1)*local_m - 1;
   if(my_rank==thread_count-1)
	   my_last_row = m-1;

   for (i = my_first_row; i <= my_last_row; i++) {
      y[i] = 0.0;
      for (j = 0; j < n; j++)
         y[i] += A[i*n+j]*x[j];
   }

   return NULL;
}  /* Mat_vect_mult */


/*-------------------------------------------------------------------
*   Generate a matrix randomly
*/

void Gen_num(
	double tar[],
	int N)
{
	int i;

	srand((unsigned)(time(NULL)));
	for(i=0;i<N;i++)
		tar[i]=rand()%MAX;

	return ;

}