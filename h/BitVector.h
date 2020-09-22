#pragma once

#include <stdint.h>
#include "dataRep.h"

#define SetBit(A,k)     ( A[(k / 8)] |= (1 << (k % 8)) )
#define ClearBit(A,k)   ( A[(k / 8)] &= ~(1 << (k % 8)) )
#define TestBit(A,k)    ( A[(k / 8)] & (1 << (k % 8)) )

class BitVector
{
public:
	BitVector(char *cluster, int cNum);
	~BitVector();

	void format();

	long long int freeClustersNum() const;

	uint32_t takeCluster();
	void freeCluster(int cluster);

	char *bitVect;
	long long int free, clusterNum;
	Sem mutex;
};

