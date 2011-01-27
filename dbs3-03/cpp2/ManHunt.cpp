#include "ManHunt.h"

void crucialEventDetected(int RegionScanNumber, int PoliceFromX, int PoliceFromY, int PoliceToX, int PoliceToY, int CriminalFromX, int CriminalFromY, int CriminalToX, int CriminalToY)
{
	printf("Crucial event detected for the #%d. region scan. \nPolice officer moves from %d,%d to %d,%d. \nCriminal comes up to police officer from %d,%d to %d,%d.\n\n", RegionScanNumber, PoliceFromX, PoliceFromY, PoliceToX, PoliceToY, CriminalFromX, CriminalFromY, CriminalToX, CriminalToY);
}

int main(int argc, char ** argv){
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
  while (CurrentMatrix = readNextAreaScan()){

	  /*****************************************************************/

	  if(CurrentMatrix) fMatrixScanner(CurrentMatrix);

	  /*****************************************************************/

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
  //crucialEventDetected(0,0,0,1,1,3,3,2,2);
  }

  /** place tmp() here! **/


#ifdef CALC_TIME
  GET_STOP_TIME;

  PRINT_TIME(stdout,freq);
#endif
  exit(0);
}

void tmp(){
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
	  for(int i=0; i < NUM_THREADS; ++i)
	    {
	      threadId[i] = i % NUM_CORES;
	    }

	  /* create threads and pass parameters: function to call and assigned CPU-number*/
	  for(int i=0; i < NUM_THREADS; ++i)
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
	  for(int i=0; i < NUM_THREADS; ++i)
	    {
	      pthread_join(thread[i],NULL);
	    }

	    /* end of example code for multithreading ... */
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

void fMatrixScanner(t_Matrix * matrix){
	// Cops appeared - 0, Cops disappeared - 1, Criminals appeared - 2, Criminals disappeared - 3
	HMap detectedEvents[4];
	//
	AMap cops[8];
	AMap criminals[8];

	fScanMatrix(matrix,detectedEvents);
	fGetMovement(detectedEvents[0],detectedEvents[1],cops);
	fGetMovement(detectedEvents[2],detectedEvents[3],criminals);

	/*
	// Matrix "besser" visualisiert
	fPrintMatrix(matrix);
	std::cout << "Cops ::~>" << std::endl;
	for(int i=0;i<8;++i){ fPrintActors(cops[i]);}
	std::cout << "Criminals ::~>" << std::endl;
	for(int i=0;i<8;++i){ fPrintActors(criminals[i]); }
	std::cout << std::endl;
	*/
	for(int i=0;i<8;++i) fDetectCrucialEvent(cops[i],criminals[7-i],i);
}

int fScanMatrix(t_Matrix * matrix, HMap (&detectedEvents)[4]){
	for (int y = 0; y < MATRIX_SIZE; y++) {
		for (int x = 0; x < MATRIX_SIZE; x++) {
			switch (fDetermineEvent(getStatus((*matrix)[y][x].statusScan),getType((*matrix)[y][x].description))) {
				case 2:
					detectedEvents[0].insert(VPair(x + MATRIX_SIZE * y, true));
					break;
				case -2:
					detectedEvents[1].insert(VPair(x + MATRIX_SIZE * y, true));
					break;
				case 3:
					detectedEvents[2].insert(VPair(x + MATRIX_SIZE * y, true));
					break;
				case -3:
					detectedEvents[3].insert(VPair(x + MATRIX_SIZE * y, true));
					break;
				default:
					break;
			}
		}
	}
	return 0;
}

int fGetMovement(const HMap &appearings, const HMap &disappearings, AMap (&actors)[8]){

	for(HMap::const_iterator it = appearings.begin();it != appearings.end();++it){
    	int richtungen[] = {(*it).first-MATRIX_SIZE-1,(*it).first-MATRIX_SIZE,(*it).first-MATRIX_SIZE+1,(*it).first+1,(*it).first-1,(*it).first+MATRIX_SIZE-1,(*it).first+MATRIX_SIZE,(*it).first+MATRIX_SIZE+1};
    	HMap::const_iterator itD;
    	for(int i=0; i<8;++i){
    		itD = disappearings.find(richtungen[i]);
    		if(itD != disappearings.end()){
    			actors[7-i].insert(APair((*it).first, (new Person2((*itD).first,(*it).first)) ));
    			// TODO: abgearbeitete loeschen!
    			break;
    		}
    	}
	}
	return 0;
}

void fDetectCrucialEvent(const AMap &cops, const AMap &criminals, int direction){
	//
	int richtungen[8] = {-(MATRIX_SIZE+1),-MATRIX_SIZE,-(MATRIX_SIZE-1),1,-1,MATRIX_SIZE-1, MATRIX_SIZE, MATRIX_SIZE+1};
	for(AMap::const_iterator it = cops.begin();it != cops.end();++it){
		//
		AMap::const_iterator itD;
		for(int i=1,j=0; j >= 0 && j <= MATRIX_SIZE*MATRIX_SIZE;++i){
			//richtungen[i]*i+(*it).first
			j = richtungen[direction]*i+(*it).first;
			itD = criminals.find(j);
			if(itD != criminals.end()){
				crucialEventDetected(0,(*it).second->from%MATRIX_SIZE,(*it).second->from/MATRIX_SIZE,(*it).second->to%MATRIX_SIZE,(*it).second->to/MATRIX_SIZE,(*itD).second->from%MATRIX_SIZE,(*itD).second->from/MATRIX_SIZE,(*itD).second->to%MATRIX_SIZE,(*itD).second->to/MATRIX_SIZE);
				// TODO: gefundenen Verbrecher loeschen
			}

		}
	}
}

// TODO: Not needed! ~> delete!?
bool fDirectionCond(int copN, int criminalN, int direction){
	switch (direction) {
		case 0: return (criminalN < copN && (abs(criminalN - copN)%(MATRIX_SIZE+1)) == 0); 	break;
		case 1: return (criminalN < copN && copN%MATRIX_SIZE == criminalN%MATRIX_SIZE);		break;
		case 2: return (criminalN < copN && (abs(criminalN - copN)%(MATRIX_SIZE-1)) == 0); 	break;
		case 3: return (criminalN > copN && copN/MATRIX_SIZE == criminalN/MATRIX_SIZE); 	break;
		case 4: return (criminalN < copN && copN/MATRIX_SIZE == criminalN/MATRIX_SIZE); 	break;
		case 5: return (criminalN > copN && (abs(criminalN - copN)%(MATRIX_SIZE-1)) == 0); 	break;
		case 6: return (criminalN > copN && copN%MATRIX_SIZE == criminalN%MATRIX_SIZE); 	break;
		case 7: return (criminalN > copN && (abs(criminalN - copN)%(MATRIX_SIZE+1))); 		break;
	}
	return false;
}

void fPrintMatrix(t_Matrix * CurrentMatrix){
	for (int i = 0; i < MATRIX_SIZE; i++)
		std::cout << "\t" << i;
	std::cout << std::endl;
	for (int i = 0; i < MATRIX_SIZE; i++)
		std::cout << "\t=";
	std::cout << std::endl;
	for (int i = 0; i < MATRIX_SIZE; i++) {
		std::cout << i << " |\t";
		for (int j = 0; j < MATRIX_SIZE; j++) {
			std::cout << fDetermineEvent(getStatus(
					(*CurrentMatrix)[i][j].statusScan), getType(
					(*CurrentMatrix)[i][j].description)) << "\t";
		}
		std::cout << std::endl;
	}
	std::cout << "_____________________________________________" << std::endl;
	std::cout << std::endl;
}

void fPrintActors(AMap &actor){
	for(AMap::const_iterator it = actor.begin();it != actor.end();++it){
		std::cout << "\t From: " << (*it).second->from%MATRIX_SIZE << "/" << (*it).second->from/MATRIX_SIZE << "; To: " << (*it).second->to%MATRIX_SIZE << "/" << (*it).second->to/MATRIX_SIZE << "; n=" << (*it).first << std::endl;
	}
}
