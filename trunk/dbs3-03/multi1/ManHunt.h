#ifndef ManHunt_H
#define ManHunt_H

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include<tr1/unordered_map>

class Person {
public:
	int from,to;
	Person(int from,int to){
		this->from = from;
		this->to = to;
	}
};

typedef std::tr1::unordered_map<int, bool> HMap;
typedef HMap::value_type VPair;
typedef std::tr1::unordered_map<int, Person*> AMap;
typedef AMap::value_type APair;
typedef int * t_Matrix;

#endif
