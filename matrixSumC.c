/* matrix summation using pthreads

   -Trådarna delar inte upp data i fasta block
   -Varje tråd hämtar nästa lediga rad mha räknaren nextRow
   - Varje tråd beräknar lokal summa, lokalt max/min och uppdaterar globala resultat.
   - Globala resultat skyddas av mutexar.

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
int numWorkers;           /* number of workers */ 
int numArrived = 0;       /* number who have arrived */

double start_time, end_time; /* start and end times */
int size, stripSize;  /* assume size is multiple of numWorkers */
int matrix[MAXSIZE][MAXSIZE]; /* matrix */

int final_sum = 0; //global summa
int max = 0;      //globalt max värde
int min = 0;    //globalt min värde
int maxRowIndex = 0; 
int maxColIndex = 0;
int minRowIndex = 0;
int minColIndex = 0;

int nextRow = 0; //räknare för nästa rad att hämta
pthread_mutex_t taskLock; //mutex för räknaren


/* timer */
double read_timer() {
    static bool initialized = false;
    static struct timeval start;
    struct timeval end;
    if( !initialized )
    {
        gettimeofday( &start, NULL ); //aktuell tid 
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

  /* initialize mutex */
  pthread_mutex_init(&resultLock, NULL);
  pthread_mutex_init(&taskLock, NULL);

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

  #ifdef DEBUG // skriv ut matrisen
  for (i = 0; i < size; i++) {
	  printf("[ ");
	  for (j = 0; j < size; j++) {
	    printf(" %d", matrix[i][j]);
	  }
	  printf(" ]\n");
  }
#endif

  max = matrix[0][0]; //Sätt initiala värden
  min = matrix[0][0];


  /* create threads, each thread takes worker function*/
  start_time = read_timer();
  for (l = 0; l < numWorkers; l++)
    pthread_create(&workerid[l], &attr, Worker, NULL); //thread id är NULL eftersom tråden hämtar rader själv
  
  for(int i = 0; i < numWorkers; i++) { //join threads after work is done
      pthread_join(workerid[i], NULL);
  }

  end_time = read_timer();

  printf("The total is %d\n", final_sum);
  printf("The max value is %d at (%d, %d)\n", max, maxRowIndex, maxColIndex);
  printf("The min value is %d at (%d, %d)\n", min, minRowIndex, minColIndex);
  printf("The execution time is %g sec\n", end_time - start_time);
}

/* Worker:
   - Hämtar nästa lediga rad via nextRow (skyddad av taskLock)
   - Beräknar lokal summa, lokalt max/min
   - Uppdaterar globala resultat under resultLock
*/

void *Worker(void *arg) {
  //Tråd sparar lokala resultat i dessa variabler
  int localSum = 0;
  int localMax, localMin;
  int localMaxRow, localMaxCol;
  int localMinRow, localMinCol;
    
  int localrow = 0; 

    while(1){ 
      
    //Hämta nästa rad, om alla rader är klara, bryt
    pthread_mutex_lock(&taskLock);
    localrow = nextRow++;
    pthread_mutex_unlock(&taskLock);
        if(localrow >= size){
            break;
    }
    
    localMax = matrix[localrow][0]; //Initiera lokala max och min värden + summa
    localMin = matrix[localrow][0];
    localMaxRow = localrow;
    localMinRow = localrow;
    localMaxCol = 0;
    localMinCol = 0;
    localSum = 0;

    //Beräkna lokal summa, lokalt max och lokalt min för raden
    for(int i = 0; i < size; i++){
        int currentValue = matrix[localrow][i];
        localSum += currentValue;

        if (currentValue > localMax) {
            localMax = currentValue;
            localMaxCol = i;
        }
        if (currentValue < localMin) {
            localMin = currentValue;
            localMinCol = i;
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
    }
    return NULL;
  }
