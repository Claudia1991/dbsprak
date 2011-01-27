#include "ManHunt.h"

void crucialEventDetected(int RegionScanNumber, int PoliceFromX, int PoliceFromY, int PoliceToX, int PoliceToY, int CriminalFromX, int CriminalFromY, int CriminalToX, int CriminalToY) {
	printf("Crucial event detected for the #%d. region scan. \nPolice officer moves from %d,%d to %d,%d. \nCriminal comes up to police officer from %d,%d to %d,%d.\n\n", RegionScanNumber, PoliceFromX, PoliceFromY, PoliceToX, PoliceToY, CriminalFromX, CriminalFromY, CriminalToX, CriminalToY);
}

char *matrixFileName;
int matrixSize = 8;
int numThreads = 8;
FILE *matrixFile;
int regionScanNumber = 0;
pthread_mutex_t mutex;

int fMatrixReader(int *matrix) {
  int tempStatus, tempDescription;
  regionScanNumber++;
  for (int i=0; i < matrixSize; i++) {
    for (int j=0; j < matrixSize; j++) {
      if(feof(matrixFile)) { return 0; }
      fscanf(matrixFile, "%d", &tempStatus);
      if(feof(matrixFile)) { return 0; }
      fscanf(matrixFile, "%d", &tempDescription);
      matrix[(i*matrixSize)+j] = (tempStatus%3)*10 + (tempDescription%4);
      // 12 = Police arrived
      // 13 = Criminal arrived
      // 22 = Police disappeared
      // 23 = Criminal disappeared
    }
  }
  return regionScanNumber;
}

void *threadFunction(void *arg) {
  int *matrix = (int*) arg;
  int matrixNum = 0;
  while (1) {
    pthread_mutex_lock(&mutex);
    matrixNum = fMatrixReader(matrix);
    if (!matrixNum) { pthread_mutex_unlock(&mutex); pthread_exit(0); }
    printf("Matrix %d:\n", matrixNum);
    for (int i=0; i < matrixSize; i++) {
      for (int j=0; j < matrixSize; j++) {
        printf("%3d", matrix[(i*matrixSize)+j]);
      }
      printf("\n");
    }
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
