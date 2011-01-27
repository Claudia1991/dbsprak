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
pthread_mutex_t matrixFileMutex;
pthread_mutex_t outputMutex;

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
      case 12: // Police appeared
        detectedEvents[0].insert(VPair(i*matrixSize + j, true));
        break;
      case 22: // Police disappeared
        detectedEvents[1].insert(VPair(i*matrixSize + j, true));
        break;
      case 13: // Criminal appeared
        detectedEvents[2].insert(VPair(i*matrixSize + j, true));
        break;
      case 23: // Criminal disappeared
        detectedEvents[3].insert(VPair(i*matrixSize + j, true));
        break;
      }
    }
  }
  return regionScanNumber;
}

int fGetMovement(const HMap &appearances, const HMap &disappearances, AMap (&actors)[8]){
  for(HMap::const_iterator it = appearances.begin(); it != appearances.end(); ++it){
    int richtungen[] = {(*it).first-matrixSize-1, (*it).first-matrixSize, (*it).first-matrixSize, (*it).first+1, (*it).first-1, (*it).first+matrixSize-1, (*it).first+matrixSize, (*it).first+matrixSize+1};
    HMap::const_iterator itD;
    for(int i=0; i<8;++i){
      itD = disappearances.find(richtungen[i]);
      if(itD != disappearances.end()){
        actors[7-i].insert(APair((*it).first, (new Person((*itD).first,(*it).first)) ));
        // TODO: abgearbeitete loeschen!
        break;
      }
    }
  }
  return 0;
}

void fDetectCrucialEvent(const AMap &cops, const AMap &criminals, int direction, int scanNum){
  int richtungen[8] = {-(matrixSize+1),-matrixSize,-(matrixSize-1),1,-1,matrixSize-1, matrixSize, matrixSize+1};
  for(AMap::const_iterator it = cops.begin();it != cops.end();++it){
    AMap::const_iterator itD;
    for(int i=1,j=0; j >= 0 && j <= matrixSize*matrixSize;++i){
      //richtungen[i]*i+(*it).first
      j = richtungen[direction]*i+(*it).first;
      itD = criminals.find(j);
      if(itD != criminals.end()){
        crucialEventDetected(scanNum,(*it).second->from%matrixSize,(*it).second->from/matrixSize,(*it).second->to%matrixSize,(*it).second->to/matrixSize,(*itD).second->from%matrixSize,(*itD).second->from/matrixSize,(*itD).second->to%matrixSize,(*itD).second->to/matrixSize);
      }
    }
  }
}

void *threadFunction(void *arg) {
//  int *matrix = (int*) arg;
  int threadID = (int) arg;
  HMap detectedEvents[4];
  AMap cops[8];
  AMap criminals[8];
  int matrixNum = 0;
  while (1) {
    pthread_mutex_lock(&matrixFileMutex);
    matrixNum = fMatrixReader(detectedEvents);
    pthread_mutex_unlock(&matrixFileMutex);
    if (!matrixNum) { pthread_exit(0); }
    fGetMovement(detectedEvents[0],detectedEvents[1],cops);
    fGetMovement(detectedEvents[2],detectedEvents[3],criminals);
    for(int i=0;i<8;++i) fDetectCrucialEvent(cops[i],criminals[7-i],i, matrixNum);
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
  if ( !matrixFile ) { printf("opening matrix file failed.\n"); exit(-1); }
  pthread_t threads[numThreads];
  if ( pthread_mutex_init(&matrixFileMutex, NULL) != 0 ) { printf("mutex creation failed.\n"); exit(-1); }
  for (int i=0; i < numThreads; i++) {
//    if( pthread_create(&threads[i], NULL, &threadFunction, *matrices[i]) != 0 ) { printf("thread creation failed.\n"); exit(-1); }
    if( pthread_create(&threads[i], NULL, &threadFunction, (int*)i) != 0 ) { printf("thread creation failed.\n"); exit(-1); }
  }
  for (int i=0; i < numThreads; i++) {
    pthread_join(threads[i], NULL);
  }
  fclose(matrixFile);
  return 0;
}
