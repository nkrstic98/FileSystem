#include "indexCluster.h"

IndexCluster::IndexCluster(char *cluster, uint32_t entry)
{
	this->index_1 = (IndexEntry*)cluster;
	this->clusterNum = entry;

	currPos = 0;
	for (int i = 0; i < INDEXSIZE; i++) {
		if(index_1->entries[i] != 0) currPos++;
		else break;
	}
}

IndexCluster::~IndexCluster() {
	delete index_1;
}

int IndexCluster::setEntry(int e)
{
	if (currPos == INDEXSIZE) return -1;

	index_1->entries[currPos++] = e;

	return 0;
}

int IndexCluster::getEntry(int e) const
{
	return index_1->entries[e];
}
