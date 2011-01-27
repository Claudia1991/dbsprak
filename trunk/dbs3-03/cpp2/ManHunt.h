#ifndef ManHunt_H
#define ManHunt_H

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#ifndef __USE_GNU
#define __USE_GNU
#endif

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <sched.h>

#include <time.h>
#include <sys/time.h>

#include<tr1/unordered_map>
#include "Matrix.h"
#include <iostream>

static inline long long int getRDTSC()
{
      long long int rc;
      asm __volatile__("rdtsc\n\t"
          "mov %%eax,(%%rsi)\n\t"
          "mov %%edx,4(%%rsi)"
         :: "S" (&rc));
      return rc;
}

static inline double getCPUFreq()
{
 struct timeval tpStart,tpStop;
 gettimeofday(&tpStart,0);

 long long int rStart,rStop;

 rStart = getRDTSC();

 sleep(2);

 rStop = getRDTSC();

 gettimeofday(&tpStop,0);

 long long int ticks = rStop-rStart;

 long tsec = tpStop.tv_sec - tpStart.tv_sec;
 long tmsec = tpStop.tv_usec - tpStart.tv_usec;

 long t = tsec * 1000000 + tmsec;

 double freq = ((double) ticks / (double) t) *1000000.0;

#ifdef _DEBUG
 printf("msec : %d ticks: %lld freq: %g GHz\n",t,freq/1E9);
#endif

 return freq;
}

#define VAR_TIME long long int startT,stopT;
#define GET_START_TIME startT = getRDTSC();
#define GET_STOP_TIME stopT = getRDTSC(); 
#define PRINT_TIME(stream,freq) { \
  if(startT <= stopT){ \
  long long int sumT = stopT - startT; \
  double time = (sumT/freq); \
  uint sec=(uint)time; time -= sec; time *= 1000; \
  uint ms= time; time -=ms; time*=1000; \
  fprintf(stream,"Cycles: %lld = %u s %u ms %.0f ns \n",sumT,sec,ms,time); \
  fflush(stream);} \
  else fprintf(stream,"Overflow in RDTSC %lld %lld\n",startT,stopT);}

#define NUM_CORES 4
#define NUM_THREADS 8
/* # define USE_MUTEX */
/* # define USE_AFFINITY */
/* # define _DEBUG */
/* # define CALC_TIME */

/* lock */
pthread_mutex_t global_mutex = PTHREAD_MUTEX_INITIALIZER;
/* signal to activate sleeping threads waiting for lock */
pthread_cond_t global_cond = PTHREAD_COND_INITIALIZER;
/* shared resource */
int sharedVar = 1;

void * PatternThreadFunc(void*);

class Person {
public:
	int fromX,fromY,toX,toY,v,n;
	Person(int fromX,int fromY,int toX,int toY,int v,int n){
		//
		this->fromX = fromX;
		this->toX = toX;
		this->fromY = fromY;
		this->toY = toY;
		this->v = v;
		this->n = n;
	}
	friend std::ostream& operator<< (std::ostream& o, const Person &c){
		return o << "FromX=" << c.fromX << ", FromY=" << c.fromY << ", ToX=" << c.toX << ", ToY=" << c.toY << ", v=" << c.v << ", N=" << c.n;
	}
};
class Person2 {
public:
	int from,to;
	Person2(int from,int to){
		//
		this->from = from;
		this->to = to;
	}
	friend std::ostream& operator<< (std::ostream& o, const Person2 &c){
		return o << "From=" << c.from;
	}
};

void fMatrixScanner(t_Matrix *);

int fDetermineEvent(int i, int j){
	if(i == 1 && j ==2) return 2; // Cop appear
	if(i == 1 && j ==3) return 3; // Criminal appear
	if(i == 2 && j ==2) return -2; // Cop disappear
	if(i == 2 && j ==3) return -3; // Criminal disappear
	return 0;
}

typedef std::tr1::unordered_map<int, bool> HMap;
typedef HMap::value_type VPair;
typedef std::tr1::unordered_map<int, Person2*> AMap;
typedef AMap::value_type APair;

int fScanMatrix(t_Matrix * matrix, HMap (&detectedEvents)[4]);
int fGetMovement(const HMap &appearings, const HMap &disappearings, AMap (&actors)[8]);
void fDetectCrucialEvent(const AMap &cops, const AMap &criminals, int direction);
void fPrintMatrix(t_Matrix *);
void fPrintActors(AMap &actor);

#endif
