#pragma once

#include <stdint.h>
#include "part.h"

const unsigned int FNAMELEN = 8;
const unsigned int FEXTLEN = 3;

struct BitVector {
	char vector[ClusterSize];
};

struct DirEntry {
	char fname[FNAMELEN] = { 0 };
	char fext[FEXTLEN] = { 0 };
	char notUsed = 0;
	uint32_t cluster;
	uint64_t fileSize;
	char free[12] = { 0 };
};

struct IndexEntry {
	uint32_t clusterNum;
};