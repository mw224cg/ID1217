/* matrix summation using pthreads

   - Varje tråd får ett tilldelat block av rader att bearbeta från matrisen
    - Varje tråd beräknar lokal summa, lokalt max/min och uppdaterar globala resultat.

   usage under Linux:
     gcc matrixSum.c -lpthread
     a.out size numWorkers

*/
#ifndef _REENTRANT 
#define _REENTRANT 
#endif 
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>
#define MAXSIZE 10000  /* maximum matrix size */
#define MAXWORKERS 10   /* maximum number of workers */


pthread_mutex_t resultLock;  /* mutex lock for the result */    
pthread_cond_t go;        /* condition variable for leaving */
int numWorkers;           /* number of workers */ 
int numArrived = 0;       /* number who have arrived */

double start_time, end_time; /* start and end times */
int size, stripSize;  /* assume size is multiple of numWorkers */
int matrix[MAXSIZE][MAXSIZE]; /* matrix */

int final_sum = 0;
int max = 0;
int min = 0;
int maxRowIndex = 0;
int maxColIndex = 0;
int minRowIndex = 0;
int minColIndex = 0;



/* timer */
double read_timer() {
    static bool initialized = false;
    static struct timeval start;
    struct timeval end;
    if( !initialized )
    {
        gettimeofday( &start, NULL );
        initialized = true;
    }
    gettimeofday( &end, NULL );
    return (end.tv_sec - start.tv_sec) + 1.0e-6 * (end.tv_usec - start.tv_usec);
    // 1.0e-6 ms till s
    // tv_ hela sekunder 
    // mirko_sekudner 
}

void *Worker(void *);

/* read command line, initialize, and create threads */
int main(int argc, char *argv[]) {
  int i, j;
  long l; /* use long in case of a 64-bit system */
  pthread_attr_t attr;
  pthread_t workerid[MAXWORKERS];

  /* set global thread attributes */
  pthread_attr_init(&attr);
  pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

  /* initialize mutex and condition variable */
  pthread_mutex_init(&resultLock, NULL);
  pthread_cond_init(&go, NULL);

  /* read command line args if any */
  size = (argc > 1)? atoi(argv[1]) : MAXSIZE;
  numWorkers = (argc > 2)? atoi(argv[2]) : MAXWORKERS;
  if (size > MAXSIZE) size = MAXSIZE;
  if (numWorkers > MAXWORKERS) numWorkers = MAXWORKERS;
  stripSize = size/numWorkers;

  /* initialize the matrix */
  for (i = 0; i < size; i++) {
	  for (j = 0; j < size; j++) {
          matrix[i][j] = rand()%99; 
	  }
  }

  max = matrix[0][0]; //Sätt initiala värden
  min = matrix[0][0];


  /* do the parallel work: create the workers */
  start_time = read_timer();
  for (l = 0; l < numWorkers; l++)
    pthread_create(&workerid[l], &attr, Worker, (void *) l);
  
  for(int i = 0; i < numWorkers; i++) {
      pthread_join(workerid[i], NULL);
  }

  end_time = read_timer();

  printf("The total is %d\n", final_sum);
  printf("The max value is %d at (%d, %d)\n", max, maxRowIndex, maxColIndex);
  printf("The min value is %d at (%d, %d)\n", min, minRowIndex, minColIndex);
  printf("The execution time is %g sec\n", end_time - start_time);
}

/* Worker:
   - Each worker processes a strip of the matrix
   - Computes local sum, local max/min
   - Updates global results under resultLock
*/
void *Worker(void *arg) {
  long myid = (long) arg;
  int i, j, first, last;

  int localSum = 0;
  int localMax, localMin;
  int localMaxRow, localMaxCol;
  int localMinRow, localMinCol;

  first = myid * stripSize;
  last = (myid == numWorkers - 1) ? (size - 1) : (first + stripSize - 1);

  localMax = localMin = matrix[first][0];
  localMaxRow = localMinRow = first;
  localMaxCol = localMinCol = 0;

  for (i = first; i <= last; i++) {
        for (j = 0; j < size; j++) {
            int currentValue = matrix[i][j];
            localSum += currentValue;

            if (currentValue > localMax) {
                localMax = currentValue;
                localMaxRow = i;
                localMaxCol = j;
            }
            if (currentValue < localMin) {
                localMin = currentValue;
                localMinRow = i;
                localMinCol = j;
            }
        }
    }

    pthread_mutex_lock(&resultLock); //Lås innan globala variabler ändras
    final_sum += localSum;

    if (localMax > max) {
        max = localMax;
        maxRowIndex = localMaxRow;
        maxColIndex = localMaxCol;
    }
    if (localMin < min) {
        min = localMin;
        minRowIndex = localMinRow;
        minColIndex = localMinCol;
    }
    pthread_mutex_unlock(&resultLock); //Lås upp efter ändring
    return NULL;
  }
