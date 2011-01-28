#ifndef ManHunt_H
#define ManHunt_H

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <sched.h>
#include <tr1/unordered_map>

#include <time.h>
#include <sys/time.h>

static inline long long int getRDTSC() {
  long long int rc;
  asm __volatile__("rdtsc\n\t"
      "mov %%eax,(%%rsi)\n\t"
      "mov %%edx,4(%%rsi)"
      :: "S" (&rc));
  return rc;
}

static inline double getCPUFreq() {
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

class Person {
public:
  int from,to,direction;
  Person(int from, int to,int direction){
    this->from = from;
    this->to = to;
    this->direction = direction;
  }
};

typedef std::tr1::unordered_map<int, bool> HMap;
typedef HMap::value_type VPair;
typedef std::tr1::unordered_map<int, Person*> AMap;
typedef AMap::value_type APair;
typedef int * t_Matrix;

#endif
