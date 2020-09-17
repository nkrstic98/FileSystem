#pragma once

#include "dataRep.h"

struct IndexEntry {
	int entries[INDEXSIZE];
};

class IndexCluster
{
public:
	IndexCluster(char* cluster);
	~IndexCluster();

	int setEntry(int e);
	int getEntry(int e) const;

	IndexEntry* index_1;
	int currPos;
};

