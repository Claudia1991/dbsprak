#include "ManHunt.h"

char *matrixFileName;
int matrixSize = 8;
int numThreads = 8;
FILE *matrixFile;
int regionScanNumber = 0;
pthread_mutex_t matrixFileMutex;

void crucialEventDetected(int scanNum, int copFrom, int copTo, int criminalFrom, int criminalTo) {
  printf("Crucial event detected for the #%d. region scan. \nPolice officer moves from %d,%d to %d,%d. \nCriminal comes up to police officer from %d,%d to %d,%d.\n\n", scanNum, copFrom%matrixSize, copFrom/matrixSize, copTo%matrixSize, copTo/matrixSize, criminalFrom%matrixSize, criminalFrom/matrixSize, criminalTo%matrixSize, criminalTo/matrixSize);
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
      case 12: // Cop appeared
        detectedEvents[0].insert(VPair(i*matrixSize + j, true));
        break;
      case 22: // Cop disappeared
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

int fGetMovement(const HMap &appearances, const HMap &disappearances, AMap (&actors)){
  for(HMap::const_iterator itA = appearances.begin(); itA != appearances.end(); ++itA){
    int directions[] = {(*itA).first-(matrixSize+1), (*itA).first-matrixSize, (*itA).first-(matrixSize-1), (*itA).first-1, (*itA).first+1, (*itA).first+matrixSize-1, (*itA).first+matrixSize, (*itA).first+matrixSize+1};
    HMap::const_iterator itD;
    for(int i=0; i<8;++i){
      itD = disappearances.find(directions[i]);
      if(itD != disappearances.end()){
        actors.insert(APair((*itA).first, (new Person((*itD).first, (*itA).first, (*itA).first-(*itD).first))));
        break;
      }
    }
  }
  return 0;
}

void fDetectCrucialEvent(const AMap &cops, const AMap &criminals, int scanNum){
  for(AMap::const_iterator itCop = cops.begin(); itCop != cops.end(); ++itCop){
    AMap::const_iterator itCriminal;
    int pruneBefore = 0, pruneAfter = 0;
    if ( ((*itCop).second->direction == -(matrixSize+1)) || ((*itCop).second->direction == -1) || ((*itCop).second->direction == matrixSize-1) ) pruneBefore = 1;
    if ( ((*itCop).second->direction == -(matrixSize-1)) || ((*itCop).second->direction == 1) || ((*itCop).second->direction == matrixSize+1) ) pruneAfter = 1;
    for(int i=1,j=0; j >= 0 && j <= matrixSize*matrixSize; ++i){
      j = (*itCop).second->to + (*itCop).second->direction*i;
      if ( pruneBefore && (j%matrixSize == 0) ) break;
      itCriminal = criminals.find(j);
      if (itCriminal != criminals.end() && (*itCop).second->direction == -(*itCriminal).second->direction){
//        printf("Scan %d: Cop from %d to %d, Criminal from %d to %d\n",scanNum, (*itCop).second->from, (*itCop).second->to, (*itCriminal).second->from, (*itCriminal).second->to);
        crucialEventDetected(scanNum, (*itCop).second->from, (*itCop).second->to, (*itCriminal).second->from, (*itCriminal).second->to);
      }
      if ( pruneAfter && (j%matrixSize == 0) ) break;
    }
  }
}

void *threadFunction(void *arg) {
  int threadID = *((int*)arg);
#ifdef USE_AFFINITY
  int cpu_id = threadID%4;
  cpu_set_t mask;
  CPU_ZERO(&mask);
  CPU_SET(cpu_id,&mask);
  sched_setaffinity(0,sizeof(mask),&mask);
#endif
  int matrixNum = 0;
  while (1) {
    HMap detectedEvents[4];
    AMap cops;
    AMap criminals;
    pthread_mutex_lock(&matrixFileMutex);
    matrixNum = fMatrixReader(detectedEvents);
    pthread_mutex_unlock(&matrixFileMutex);
    if (!matrixNum) { pthread_exit(0); }
    fGetMovement(detectedEvents[0],detectedEvents[1],cops);
    fGetMovement(detectedEvents[2],detectedEvents[3],criminals);
    fDetectCrucialEvent(cops, criminals, matrixNum);
  }
  pthread_exit(0);
}

int main(int argc, char ** argv){
  // ManHunt "MatrixFile" "matrixSize" "numThreads"
  struct timeval tpStart,tpStop;
  gettimeofday(&tpStart,0);
#ifdef CALC_TIME
  int cpu_id = 0;
  cpu_set_t mask;
  CPU_ZERO(&mask);
  CPU_SET(cpu_id,&mask);
  sched_setaffinity(0,sizeof(mask),&mask);
  double freq = getCPUFreq();
  VAR_TIME;
  GET_START_TIME;
#endif
  switch(argc) {
  case 4:
    numThreads = atoi(argv[3]);
  case 3:
    matrixSize =  atoi(argv[2]);
  case 2:
    matrixFileName = (char*)malloc(strlen(argv[1])+1);
    strcpy(matrixFileName, argv[1]);
    break;
  case 1:
    matrixFileName = (char*)malloc(strlen("Matrix.in")+1);
    strcpy(matrixFileName, "Matrix.in");
    break;
  }
  printf("matrixFileName: \"%s\", matrixSize: %d, numThreads: %d\n\n", matrixFileName, matrixSize, numThreads);
  matrixFile = fopen(matrixFileName, "r");
  if ( !matrixFile ) { printf("opening matrix file failed.\n"); exit(-1); }
  free(matrixFileName);
  pthread_t threads[numThreads];
  if ( pthread_mutex_init(&matrixFileMutex, NULL) != 0 ) { printf("mutex creation failed.\n"); exit(-1); }
  int threadids[numThreads];
  for (int i=0; i < numThreads; i++) {
    threadids[i]=i;
    if( pthread_create(&threads[i], NULL, &threadFunction, &threadids[i]) != 0 ) { printf("thread creation failed.\n"); exit(-1); }
  }
  for (int i=0; i < numThreads; i++) {
    pthread_join(threads[i], NULL);
  }
  fclose(matrixFile);
#ifdef CALC_TIME
  GET_STOP_TIME;
  PRINT_TIME(stdout,freq);
#endif
  exit(0);
}
