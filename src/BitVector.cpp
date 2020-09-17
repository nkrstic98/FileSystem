#include "BitVector.h"
#include "dataRep.h"

BitVector::BitVector(char * cluster, int cNum) : bitVect(cluster), clusterNum(cNum) {
	free = 0;
	for (int i = 0; i < cNum * 8; i++) {
		if (TestBit(bitVect, i)) free++;
	}
}

BitVector::~BitVector() {}

void BitVector::format()
{
	for (int i = 0; i < BITNUM; i++)
		SetBit(this->bitVect, i); //postavi sve bitove na 1

	ClearBit(this->bitVect, 0); //zauzmi bit 0 za BitVect klaster
	ClearBit(this->bitVect, 1); //zauzmi bit 1 da klsater indeksa direktorijuma

	free = clusterNum - 2;
}

int BitVector::freeClustersNum() const
{
	return free;
}

uint32_t BitVector::takeCluster()
{
	for (int i = 0; i < clusterNum * 8; i++) {
		if (TestBit(bitVect, i)) {
			ClearBit(bitVect, i);
			return i;
		}
	}

	return 0;
}

void BitVector::freeCluster(int cluster)
{
	SetBit(bitVect, cluster);
}
