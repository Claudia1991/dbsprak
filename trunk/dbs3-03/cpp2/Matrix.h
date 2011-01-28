#ifndef Matrix_H
#define Matrix_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define MATRIX_SIZE 8
#define NUMBER_OF_SCANS 1000000

typedef struct Entry {
	int 	statusScan;		 /* encodes the arrival, disappearance, and status quo of a person in a cell */
	int		description;     /* encodes the type of a person */
} Entry;

typedef Entry t_Matrix[MATRIX_SIZE][MATRIX_SIZE];

char initialize(char * MatrixFileName);

t_Matrix* readNextAreaScan();

void printMatrix(t_Matrix* Matrix);

int getStatus(int statusScan);

void printStatus(int Status);

int getType(int description);

void printType(int Type);

#endif
