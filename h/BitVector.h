#pragma once

#include <stdint.h>

#define SetBit(A,k)     ( A[(k / 8)] |= (1 << (k % 8)) )
#define ClearBit(A,k)   ( A[(k / 8)] &= ~(1 << (k % 8)) )
#define TestBit(A,k)    ( A[(k / 8)] & (1 << (k % 8)) )

class BitVector
{
public:
	BitVector(char *cluster, int cNum);
	~BitVector();

	void format();

	int freeClustersNum() const;

	uint32_t takeCluster();
	void freeCluster(int cluster);

	char *bitVect;
	int free, clusterNum;
};

