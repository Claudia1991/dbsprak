#include "ManHunt.h"

void crucialEventDetected(int RegionScanNumber, int PoliceFromX, int PoliceFromY, int PoliceToX, int PoliceToY, int CriminalFromX, int CriminalFromY, int CriminalToX, int CriminalToY) {
	printf("Crucial event detected for the #%d. region scan. \nPolice officer moves from %d,%d to %d,%d. \nCriminal comes up to police officer from %d,%d to %d,%d.\n\n", RegionScanNumber, PoliceFromX, PoliceFromY, PoliceToX, PoliceToY, CriminalFromX, CriminalFromY, CriminalToX, CriminalToY);
}

char *matrixFileName;
int matrixSize = 8;
int MATRIX_SIZE = matrixSize;
int numThreads = 8;
FILE *matrixFile;
int regionScanNumber = 0;
pthread_mutex_t mutex;

int fDetermineEvent(int i, int j){
	if(i == 1 && j ==2) return 2; // Cop appear
	if(i == 1 && j ==3) return 3; // Criminal appear
	if(i == 2 && j ==2) return -2; // Cop disappear
	if(i == 2 && j ==3) return -3; // Criminal disappear
	return 0;
}

int fMatrixReader(HMap (&detectedEvents)[4]) {
  int tempStatus, tempDescription;
  regionScanNumber++;
  for (int i=0; i < matrixSize; i++) {
    for (int j=0; j < matrixSize; j++) {
      if(feof(matrixFile)) { return 0; }
      fscanf(matrixFile, "%d", &tempStatus);
      if(feof(matrixFile)) { return 0; }
      fscanf(matrixFile, "%d", &tempDescription);
      //matrix[(i*matrixSize)+j] = (tempStatus%3)*10 + (tempDescription%4);
      switch ((tempStatus%3)*10 + (tempDescription%4)) {
			case 12:
				detectedEvents[0].insert(VPair(j + MATRIX_SIZE * i, true));
				break;
			case 22:
				detectedEvents[1].insert(VPair(j + MATRIX_SIZE * i, true));
				break;
			case 13:
				detectedEvents[2].insert(VPair(j + MATRIX_SIZE * i, true));
				break;
			case 23:
				detectedEvents[3].insert(VPair(j + MATRIX_SIZE * i, true));
				break;
			default:
				break;
			}
      // 12 = Police arrived
      // 13 = Criminal arrived
      // 22 = Police disappeared
      // 23 = Criminal disappeared
    }
  }
  return regionScanNumber;
}

int fGetMovement(const HMap &appearings, const HMap &disappearings, AMap (&actors)[8]){

	for(HMap::const_iterator it = appearings.begin();it != appearings.end();++it){
    	int richtungen[] = {(*it).first-MATRIX_SIZE-1,(*it).first-MATRIX_SIZE,(*it).first-MATRIX_SIZE+1,(*it).first+1,(*it).first-1,(*it).first+MATRIX_SIZE-1,(*it).first+MATRIX_SIZE,(*it).first+MATRIX_SIZE+1};
    	HMap::const_iterator itD;
    	for(int i=0; i<8;++i){
    		itD = disappearings.find(richtungen[i]);
    		if(itD != disappearings.end()){
    			actors[7-i].insert(APair((*it).first, (new Person((*itD).first,(*it).first)) ));
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
				crucialEventDetected(regionScanNumber,(*it).second->from%MATRIX_SIZE,(*it).second->from/MATRIX_SIZE,(*it).second->to%MATRIX_SIZE,(*it).second->to/MATRIX_SIZE,(*itD).second->from%MATRIX_SIZE,(*itD).second->from/MATRIX_SIZE,(*itD).second->to%MATRIX_SIZE,(*itD).second->to/MATRIX_SIZE);
				// TODO: gefundenen Verbrecher loeschen
			}

		}
	}
}

void *threadFunction(void *arg) {
  int *matrix = (int*) arg;
  HMap detectedEvents[4];
  AMap cops[8];
  AMap criminals[8];
  int matrixNum = 0;
  while (1) {
    pthread_mutex_lock(&mutex);
    matrixNum = fMatrixReader(detectedEvents);
    if (!matrixNum) { pthread_mutex_unlock(&mutex); pthread_exit(0); }
    /*
    printf("Matrix %d:\n", matrixNum);
    for (int i=0; i < matrixSize; i++) {
      for (int j=0; j < matrixSize; j++) {
        printf("%3d", matrix[(i*matrixSize)+j]);
      }
      printf("\n");
    }
    */
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
    pthread_mutex_unlock(&mutex);
  }
  pthread_exit(0);
}

int main(int argc, char ** argv){
  // ManHunt "MatrixFile" "matrixSize" "numThreads"
  switch(argc) {
  case 4:
    numThreads = atoi(argv[3]);
  case 3:
    matrixSize =  atoi(argv[2]);
  case 2:
    matrixFileName = (char*)malloc(sizeof(argv[1]));
    strcpy(matrixFileName, argv[1]);
    break;
  case 1:
    matrixFileName = (char*)malloc(sizeof("Matrix.in"));
    strcpy(matrixFileName, "Matrix.in");
    break;
  }
  printf("matrixFileName: \"%s\", matrixSize: %d, numThreads: %d\n\n", matrixFileName, matrixSize, numThreads);
  int matrices[numThreads][matrixSize][matrixSize];
  matrixFile = fopen(matrixFileName, "r");
  pthread_t threads[numThreads];
  for (int i=0; i < numThreads; i++) {
    if( (pthread_create(&threads[i], NULL, &threadFunction, *matrices[i])) ) { printf("thread creation failed.\n"); exit(-1); }
  }
  for (int i=0; i < numThreads; i++) {
    pthread_join(threads[i], NULL);
  }
  fclose(matrixFile);
  return 0;
}
