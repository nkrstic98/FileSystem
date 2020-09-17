#include "indexCluster.h"

IndexCluster::IndexCluster(char *cluster) : currPos(0)
{
	this->index_1 = (IndexEntry*)cluster;
}

IndexCluster::~IndexCluster() {}

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
