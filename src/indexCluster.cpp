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

	mutex = CreateSemaphore(0, 1, 1, NULL);
}

IndexCluster::~IndexCluster() {
	delete index_1;
}

int IndexCluster::setEntry(int e)
{
	wait(mutex);

	if (currPos == INDEXSIZE) {
		signal(mutex);
		return -1;
	}

	index_1->entries[currPos++] = e;

	signal(mutex);

	return 0;
}

int IndexCluster::getEntry(int e) const
{
	return index_1->entries[e];
}
