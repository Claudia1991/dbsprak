#ifndef ManHunt_H
#define ManHunt_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>

typedef struct t_presence {
  int freeid;
  int coords[4][2];
} t_presence;

typedef struct t_movement {
  int freeid;
  int coords[8][4];
} t_movement;

#endif
