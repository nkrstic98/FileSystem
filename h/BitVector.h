#pragma once

#define SetBit(A,k)     ( A[(k / 8)] |= (1 << (k % 8)) )
#define ClearBit(A,k)   ( A[(k / 8)] &= ~(1 << (k % 8)) )
#define TestBit(A,k)    ( A[(k / 8)] & (1 << (k % 8)) )

class BitVector
{
public:
	BitVector(char *cluster, int cNum);
	~BitVector();

	void format();

	int freeClusters() const;

	char *bitVect;
	int free, clusterNum;
};

