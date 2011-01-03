#include "ManHunt.h"
#include "Matrix.h"

void crucialEventDetected(int RegionScanNumber, int PoliceFromX, int PoliceFromY, int PoliceToX, int PoliceToY, int CriminalFromX, int CriminalFromY, int CriminalToX, int CriminalToY)
{
	printf("Crucial event detected for the #%d. region scan. \nPolice officer moves from %d,%d to %d,%d. \nCriminal comes up to police officer from %d,%d to %d,%d.\n\n", RegionScanNumber, PoliceFromX, PoliceFromY, PoliceToX, PoliceToY, CriminalFromX, CriminalFromY, CriminalToX, CriminalToY);
}

int main(int argc, char ** argv)
{
  /* */
  int i,j;
  if (argc < 2)
  {
	printf("Usage:\n ManHunt <MatrixInputFileName>\n");
	exit(0);
  }
  
  if (!initialize(argv[1])) exit(0);

#ifdef CALC_TIME
  /* assign main thread to one core for correctly measuring running time */
  int cpu_id = 0;
  cpu_set_t mask;
  CPU_ZERO(&mask);
  CPU_SET(cpu_id,&mask);
  sched_setaffinity(0,sizeof(mask),&mask);

  double freq = getCPUFreq();

  VAR_TIME;

  /* start timer */
  GET_START_TIME;
#endif

  t_Matrix * CurrentMatrix;

  /* step: scan region and evaluate current matrix */

  /* get next Matrix (region scan) */
  /* returns Null if their is no further matrix (the manhunt ends) */
  while (CurrentMatrix = readNextAreaScan())
  {
  /* example for printing the content of the current Matrix */
#ifdef _DEBUG
  printMatrix(CurrentMatrix);
#endif

  /* scan Matrix */
  /* 
  get the presence of a person in a matrix cell 
  0 = no cahnge (no movement)
  1 = arrival of a person
  2 = disappearance of a person
  */
  /* example for printing the status of all cells of the current matrix */
#ifdef _DEBUG
  if (CurrentMatrix)
  {
	  for (i=0; i < MATRIX_SIZE; i++)
	  {
		for (j=0; j < MATRIX_SIZE; j++)
		{
		  printStatus(getStatus((*CurrentMatrix)[i][j].statusScan));
		}
	  }
  }
#endif

  /* identify types of recognized persons */
  /* 
  get the person type of one matrix cell 
  0 = no type (no person)
  1 = innocent
  2 = police officer
  3 = criminal
  */
  /* example for printing the types of all cells with moving persons of the current matrix */
#ifdef _DEBUG
  if (CurrentMatrix)
  {
	  for (i=0; i < MATRIX_SIZE; i++)
	  {
		for (j=0; j < MATRIX_SIZE; j++)
		{
		  if (getStatus((*CurrentMatrix)[i][j].statusScan)) printType(getType((*CurrentMatrix)[i][j].description));
		}
	  }
  }
#endif

  /* calculate movements */
  /* ... */

  /* scan for crucial event */
  /* ... */
  /* 
  send message if a crucial event was detected 
  PoliceOfficer from: x,y , to: x,y , Criminal from: x,y to: x,y
  */
  crucialEventDetected(0,0,0,1,1,3,3,2,2);
  }

  /* start of example code for multithreading ... */
	/* array of threads */
  pthread_t thread[NUM_THREADS];
	/* array to store thread to CPU assignment */
  int threadId[NUM_THREADS];

	/* distributing threads on cores
	4 CPU a 2 Cores->
	Thread 0: CPU 0 Core 0
	Thread 1: CPU 0 Core 1
	Thread 2: CPU 1 Core 0
	Thread 3: CPU 1 Core 1
	Thread 4: CPU 2 Core 0
	...
	*/
  for(i=0; i < NUM_THREADS; ++i)
    {
      threadId[i] = i % NUM_CORES;
    }
 
  /* create threads and pass parameters: function to call and assigned CPU-number*/
  for(i=0; i < NUM_THREADS; ++i)
    {
      if(pthread_create(&thread[i],NULL,&PatternThreadFunc,&threadId[i])!=0)
		{
	  perror(NULL);
	  exit(-1);
		}
    }
  /* set shared resource to be ready for use */
  sharedVar = 0;

  /* wait until threat i finishes */
  for(i=0; i < NUM_THREADS; ++i)
    {
      pthread_join(thread[i],NULL);
    }

    /* end of example code for multithreading ... */

#ifdef CALC_TIME
  GET_STOP_TIME;

  PRINT_TIME(stdout,freq);
#endif

  exit(0);
}

void * PatternThreadFunc(void* ptr)
{
#ifdef USE_AFFINITY
/*
  assign current thread to CPU number ptr
*/
 int cpu_id = *((int*)ptr);
 cpu_set_t mask;
 CPU_ZERO(&mask);
 CPU_SET(cpu_id,&mask);
#ifdef _DEBUG
 fprintf(stderr,"Set Affinity to %d thread %#08X\n",cpu_id,pthread_self());
#endif
 sched_setaffinity(0,sizeof(mask),&mask);
#endif

#ifdef USE_MUTEX
  /*
  wait for the lock 
  thread runs in a cycle -> waste CPU cycles
  */
  pthread_mutex_lock(&global_mutex);

  /* 
  get lock 
  resource available? 
  */
  while(sharedVar!=0)
    {
	  /*
	  resource in use
	  unlock and wait (sleep) for the lock of the shared resource
	  thread suspended
	  */
    pthread_cond_wait(&global_cond,&global_mutex);
    }

  /* set resource to be in use */
  sharedVar = 1;

  /*
  unlock 
  */
  pthread_mutex_unlock(&global_mutex);
#ifdef _DEBUG
  fprintf(stdout,"Working on shared resource, thread %#08X. Wait 2sec ... ",pthread_self());
#endif
#endif

#ifdef _DEBUG
#ifndef USE_MUTEX
  fprintf(stdout,"Working, thread %#08X. Wait 2sec ... ",pthread_self());
#endif
#endif
  sleep(2);
#ifdef _DEBUG
  fprintf(stdout,"finished wait\n");
#endif
#ifdef USE_MUTEX
  /* set resource to be free */
  sharedVar = 0;
  /* 
  send signal to suspended threads to wake up 
  -> these threads try to lock and test if the resource is free 
  first come first serve
  */
  pthread_cond_broadcast(&global_cond);
#endif
  pthread_exit(NULL);
}
