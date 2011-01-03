#include "Matrix.h"

t_Matrix* *AreaScans;
int actualScanNumber;
int ScanNumber = 0;

char initialize(char * MatrixFileName)
{
	char Line[500];

	FILE *MatrixFile;

	MatrixFile = fopen(MatrixFileName,"r");
	if(!MatrixFile) return 0;

	actualScanNumber = 0;
	ScanNumber = NUMBER_OF_SCANS;
	t_Matrix* NextMatrix;

	/* it's important to get one big block for matrixes in memory */
	NextMatrix = (t_Matrix*)malloc(sizeof(t_Matrix)*ScanNumber);
	AreaScans = (t_Matrix**)malloc(sizeof(t_Matrix*)*ScanNumber);
	AreaScans[0] = NextMatrix;

	int z,i,j;
	for (z=0; z<ScanNumber; z++)
	{
		AreaScans[z] = AreaScans[0] + z;
		NextMatrix = AreaScans[z];

/*#ifdef _DEBUG
		printf("Matrix %d: \n", z);
#endif*/
		for (i=0; i < MATRIX_SIZE; i++)
		{
			for (j=0; j < MATRIX_SIZE; j++)
			{
				if(feof(MatrixFile)) 
				{
					fclose(MatrixFile);
					return 0;
				}
				fscanf(MatrixFile,"%d",&(*NextMatrix)[i][j].statusScan);
/*#ifdef _DEBUG
			printf("%d ",(*NextMatrix)[i][j].statusScan);
#endif*/
				if(feof(MatrixFile)) 
				{
					fclose(MatrixFile);
					return 0;
				}
				fscanf(MatrixFile,"%d",&(*NextMatrix)[i][j].description);
/*#ifdef _DEBUG
			printf("%d ",(*NextMatrix)[i][j].description);
#endif*/
/*				(*NextMatrix)[i][j].statusScan=z*i*j;
				(*NextMatrix)[i][j].description=z*(j-i)*(j-i);	*/
			}
/*#ifdef _DEBUG
			printf("\n");
#endif*/
		}
	}
	fclose(MatrixFile);
	return 1;
}

t_Matrix* readNextAreaScan()
{
	if (actualScanNumber < ScanNumber)	
		return AreaScans[actualScanNumber++];
	else
		return NULL;
}

void printMatrix(t_Matrix* Matrix)
{
	if(!Matrix) return;

	fprintf(stdout, "Cell: x,y, Status, Description\n");
	int i,j;
  for (i=0; i < MATRIX_SIZE; i++)
  {
	for (j=0; j < MATRIX_SIZE; j++)
	{
/*		fprintf(stdout, " C %d, %d: S: %d, D: %d", i, j, (*Matrix)[i][j].statusScan, (*Matrix)[i][j].description); */
		fprintf(stdout, " S: %d, D: %d", (*Matrix)[i][j].statusScan, (*Matrix)[i][j].description);
	}
	fprintf(stdout, "\n");
  }
}

int getStatus(int statusScan)
{
	return (statusScan % 3);
}

void printStatus(int Status)
{
  switch (Status)
  {
  case 0:
	  fprintf(stdout, "Status of matrix cell: no change\n");
	  break;
  case 1:
	  fprintf(stdout, "Status of matrix cell: arrival of a person\n");
	  break;
  case 2:
	  fprintf(stdout, "Status of matrix cell: disappearance of a person\n");
	  break;
  }
}

int getType(int description)
{
	return (description % 4);
}

void printType(int Type)
{
  switch (Type)
  {
  case 0:
	  fprintf(stdout, "Person type in matrix cell: no type\n");
	  break;
  case 1:
	  fprintf(stdout, "Person type in matrix cell: innocent\n");
	  break;
  case 2:
	  fprintf(stdout, "Person type in matrix cell: police officer\n");
	  break;
  case 3:
	  fprintf(stdout, "Person type in matrix cell: criminal\n");
	  break;
  }
}
