#include "ManHunt.h"

void crucialEventDetected(int RegionScanNumber, int PoliceFromX, int PoliceFromY, int PoliceToX, int PoliceToY, int CriminalFromX, int CriminalFromY, int CriminalToX, int CriminalToY)
{
	printf("Crucial event detected for the #%d. region scan. \nPolice officer moves from %d,%d to %d,%d. \nCriminal comes up to police officer from %d,%d to %d,%d.\n\n", RegionScanNumber, PoliceFromX, PoliceFromY, PoliceToX, PoliceToY, CriminalFromX, CriminalFromY, CriminalToX, CriminalToY);
}

int main(int argc, char ** argv){
  int i,j;
  if (argc < 2){
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

  // step: scan region and evaluate current matrix, get next Matrix (region scan), returns Null if their is no further matrix (the manhunt ends)
  std::vector<int> threadId;
  std::vector<pthread_t> thread;
  sharedVar = 0;
  int matrixCount = 0;
  while (CurrentMatrix = readNextAreaScan()){
	  if(CurrentMatrix){
		  //fPrintMatrix(CurrentMatrix); // Print nice matrix (-2/2 ~ dis-/appearing Cop, -3/3 ~ dis-/appearing Crime)
		  pthread_t threadi;
		  if(pthread_create(&threadi,NULL,&PatternThreadFunc,(new ThreadContainer(CurrentMatrix,matrixCount++)))!=0){
			  perror(NULL);
			  exit(-1);
		  } else {
			  thread.push_back(threadi);
		  }
	  }
	  // max-Thread-anzahl erreicht, warten bis der 1. Thread beendet wurde und danach weitermachen
	  while(sharedVar > NUM_THREADS - 1){
		  pthread_join(thread.front(),NULL);
		  thread.erase(thread.begin());
	  }
  }
  for(int i=0; i < thread.size(); ++i){
	  pthread_join(thread[i],NULL);
  }
  thread.empty();

#ifdef CALC_TIME
  GET_STOP_TIME;

  PRINT_TIME(stdout,freq);
#endif
  exit(0);
}

void * PatternThreadFunc(void* ptr){
	ThreadContainer * cur = ((ThreadContainer*)ptr);
	t_Matrix * matrix = cur->matrix;
	int region = cur->scannedRegion;
	delete(cur);

#ifdef USE_MUTEX
  // wait for the lock thread runs in a cycle -> waste CPU cycles
  pthread_mutex_lock(&global_mutex);

  //get lock, resource available?
  while(sharedVar > NUM_THREADS - 1){
	  //std::cout << "scheduled Thread" << std::endl;
	  //resource in use, unlock and wait (sleep) for the lock of the shared resource, thread suspended
	  pthread_cond_wait(&global_cond,&global_mutex);
  }

  sharedVar++;
  //std::cout << "currently running Threads = " << sharedVar << std::endl;
  /* unlock */
  pthread_mutex_unlock(&global_mutex);

  fMatrixScanner(matrix,region);

  /* set resource to be free */
  sharedVar--;
  /* 
  send signal to suspended threads to wake up 
  -> these threads try to lock and test if the resource is free 
  first come first serve
  */
  pthread_cond_broadcast(&global_cond);
#endif
  pthread_exit(NULL);
}

void fMatrixScanner(t_Matrix * matrix, int region){
	// Cops appeared - 0, Cops disappeared - 1, Criminals appeared - 2, Criminals disappeared - 3
	HMap detectedEvents[4];
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
	for(int i=0;i<8;++i) fDetectCrucialEvent(cops[i],criminals[7-i],i,region);
	// Delete all created Objects
	for(int i=0;i<8;++i){
		for(AMap::const_iterator it = cops[i].begin();it != cops[i].end();++it) delete(it->second);
		for(AMap::const_iterator it = criminals[i].begin();it != criminals[i].end();++it) delete(it->second);
	}
}

int fScanMatrix(t_Matrix * matrix, HMap (&detectedEvents)[4]){
	for (int y = 0; y < MATRIX_SIZE; y++) {
		for (int x = 0; x < MATRIX_SIZE; x++) {
			switch (fDetermineEvent(getStatus((*matrix)[y][x].statusScan),getType((*matrix)[y][x].description))) {
				case 2: detectedEvents[0].insert(VPair(x + MATRIX_SIZE * y, true)); break;
				case -2: detectedEvents[1].insert(VPair(x + MATRIX_SIZE * y, true)); break;
				case 3: detectedEvents[2].insert(VPair(x + MATRIX_SIZE * y, true)); break;
				case -3: detectedEvents[3].insert(VPair(x + MATRIX_SIZE * y, true)); break;
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
    			actors[7-i].insert(APair((*it).first, (new Person((*itD).first,(*it).first)) ));
    			// TODO: abgearbeitete aus der HMap loeschen um doppelungen zu vermeiden (dh. ein verschwinden sollte nicht 2x [oder mehr] benutzt werden)!
    			break;
    		}
    	}
	}
	return 0;
}

void fDetectCrucialEvent(const AMap &cops, const AMap &criminals, int direction, int region){
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
				crucialEventDetected(region,(*it).second->from%MATRIX_SIZE,(*it).second->from/MATRIX_SIZE,(*it).second->to%MATRIX_SIZE,(*it).second->to/MATRIX_SIZE,(*itD).second->from%MATRIX_SIZE,(*itD).second->from/MATRIX_SIZE,(*itD).second->to%MATRIX_SIZE,(*itD).second->to/MATRIX_SIZE);
			}

		}
	}
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
