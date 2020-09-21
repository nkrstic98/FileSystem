#pragma once

#include "dataRep.h"

struct IndexEntry {
	int entries[INDEXSIZE];
};

class IndexCluster
{
public:
	IndexCluster(char* cluster, uint32_t entry);
	~IndexCluster();

	int setEntry(int e);
	int getEntry(int e) const;

	IndexEntry* index_1;
	uint32_t clusterNum;
	int currPos;
};

