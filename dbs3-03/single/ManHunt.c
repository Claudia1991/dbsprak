#include "ManHunt.h"
#include "Matrix.h"

void crucialEventDetected(int RegionScanNumber, int PoliceFromX, int PoliceFromY, int PoliceToX, int PoliceToY, int CriminalFromX, int CriminalFromY, int CriminalToX, int CriminalToY)
{
  printf("Crucial event detected for the #%d. region scan. \nPolice officer moves from %d,%d to %d,%d. \nCriminal comes up to police officer from %d,%d to %d,%d.\n\n", RegionScanNumber, PoliceFromX, PoliceFromY, PoliceToX, PoliceToY, CriminalFromX, CriminalFromY, CriminalToX, CriminalToY);
}

int main(int argc, char ** argv)
{
  struct timeval tpStart,tpStop;
  gettimeofday(&tpStart,0);
  if (argc < 2) {
    printf("Using default \"Matrix.in\" file\n\n");
    if (!initialize("Matrix.in")) { printf("unable to initialize matrix file, exiting now\n"); exit(-1); }
  }
  else {
    if (!initialize(argv[1])) { printf("unable to initialize matrix file, exiting now\n"); exit(-1); }
  }
  t_Matrix * CurrentMatrix;
  int row, col;
  int scan = 1;

  while ((CurrentMatrix = readNextAreaScan())) {
    t_presence *arrivalsPolice = malloc(sizeof(t_presence));
    arrivalsPolice->freeid = 0;
    t_presence *arrivalsCriminals = malloc(sizeof(t_presence));
    arrivalsCriminals->freeid = 0;
    t_presence *disappearancesPolice = malloc(sizeof(t_presence));
    disappearancesPolice->freeid = 0;
    t_presence *disappearancesCriminals = malloc(sizeof(t_presence));
    disappearancesCriminals->freeid = 0;

    for (row=0; row < MATRIX_SIZE; row++) {
      for (col=0; col < MATRIX_SIZE; col++) {
        switch (getStatus((*CurrentMatrix)[row][col].statusScan)) {
        case 1:
          switch (getType((*CurrentMatrix)[row][col].description)) {
          case 2:
            arrivalsPolice->coords[arrivalsPolice->freeid][0] = row;
            arrivalsPolice->coords[arrivalsPolice->freeid][1] = col;
            arrivalsPolice->freeid++;
            break;
          case 3:
            arrivalsCriminals->coords[arrivalsCriminals->freeid][0] = row;
            arrivalsCriminals->coords[arrivalsCriminals->freeid][1] = col;
            arrivalsCriminals->freeid++;
            break;
          }
          break;
        case 2:
          switch (getType((*CurrentMatrix)[row][col].description)) {
          case 2:
            disappearancesPolice->coords[disappearancesPolice->freeid][0] = row;
            disappearancesPolice->coords[disappearancesPolice->freeid][1] = col;
            disappearancesPolice->freeid++;
            break;
          case 3:
            disappearancesCriminals->coords[disappearancesCriminals->freeid][0] = row;
            disappearancesCriminals->coords[disappearancesCriminals->freeid][1] = col;
            disappearancesCriminals->freeid++;
            break;
          }
          break;
        }
      }
    }

    int i,j;
    int xTo, yTo, xFrom, yFrom;
    t_movement *movementsPolice = malloc(sizeof(t_movement));
    movementsPolice->freeid = 0;
    for (i=0; i < arrivalsPolice->freeid; i++) {
      xTo = arrivalsPolice->coords[i][0];
      yTo = arrivalsPolice->coords[i][1];
      for (j=0; j < disappearancesPolice->freeid; j++) {
        xFrom = disappearancesPolice->coords[j][0];
        yFrom = disappearancesPolice->coords[j][1];
        switch (xTo - xFrom) {
        case -1:
        case 0:
        case 1:
          switch (yTo - yFrom) {
          case -1:
          case 0:
          case 1:
            movementsPolice->coords[movementsPolice->freeid][0] = xFrom;
            movementsPolice->coords[movementsPolice->freeid][1] = yFrom;
            movementsPolice->coords[movementsPolice->freeid][2] = xTo - xFrom;
            movementsPolice->coords[movementsPolice->freeid][3] = yTo - yFrom;
            movementsPolice->freeid++;
            break;
          }
          break;
        }
      }
    }
    t_movement *movementsCriminals = malloc(sizeof(t_movement));
    movementsCriminals->freeid = 0;
    for (i=0; i < arrivalsCriminals->freeid; i++) {
      xTo = arrivalsCriminals->coords[i][0];
      yTo = arrivalsCriminals->coords[i][1];
      for (j=0; j < disappearancesCriminals->freeid; j++) {
        xFrom = disappearancesCriminals->coords[j][0];
        yFrom = disappearancesCriminals->coords[j][1];
        switch (xTo - xFrom) {
        case -1:
        case 0:
        case 1:
          switch (yTo - yFrom) {
          case -1:
          case 0:
          case 1:
            movementsCriminals->coords[movementsCriminals->freeid][0] = xFrom;
            movementsCriminals->coords[movementsCriminals->freeid][1] = yFrom;
            movementsCriminals->coords[movementsCriminals->freeid][2] = xTo - xFrom;
            movementsCriminals->coords[movementsCriminals->freeid][3] = yTo - yFrom;
            movementsCriminals->freeid++;
            break;
          }
          break;
        }
      }
    }

    free(arrivalsPolice);
    free(arrivalsCriminals);
    free(disappearancesPolice);
    free(disappearancesCriminals);

    int k;
    int xFromPolice, yFromPolice, xDirectionPolice, yDirectionPolice;
    int xFromCriminal, yFromCriminal, xDirectionCriminal, yDirectionCriminal;
    for (i=0; i < movementsPolice->freeid; i++) {
      xFromPolice = movementsPolice->coords[i][0];
      yFromPolice = movementsPolice->coords[i][1];
      xDirectionPolice = movementsPolice->coords[i][2];
      yDirectionPolice = movementsPolice->coords[i][3];
      for (j=0; j < movementsCriminals->freeid; j++) {
        xFromCriminal = movementsCriminals->coords[j][0];
        yFromCriminal = movementsCriminals->coords[j][1];
        xDirectionCriminal = movementsCriminals->coords[j][2];
        yDirectionCriminal = movementsCriminals->coords[j][3];
        for (k=2; k<6; k++) {
          if ( (xFromPolice + xDirectionPolice*k) == (xFromCriminal + xDirectionCriminal) ) {
            if ( (yFromPolice + yDirectionPolice*k) == (yFromCriminal + yDirectionCriminal) ) {
              if ( (xDirectionPolice == -xDirectionCriminal) && (yDirectionPolice == -yDirectionCriminal) ) crucialEventDetected(scan, xFromPolice, yFromPolice, xFromPolice+xDirectionPolice, yFromPolice+yDirectionPolice, xFromCriminal, yFromCriminal, xFromCriminal+xDirectionCriminal, yFromCriminal+yDirectionCriminal);
            }
          }
        }
      }
    }

    free(movementsPolice);
    free(movementsCriminals);
    scan++;
  }
  gettimeofday(&tpStop,0);
  long tsec = tpStop.tv_sec - tpStart.tv_sec;
  long tmsec = tpStop.tv_usec - tpStart.tv_usec;
  long t = tsec * 1000000 + tmsec;
  printf("msec: %ld\n",t);
  return 0;
}
