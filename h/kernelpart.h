#pragma once

#include "part.h"

class PartitionImpl
{
public:
	PartitionImpl(char* partition);
	~PartitionImpl();

	virtual ClusterNo getNumOfClusters() const;

	virtual int readCluster(ClusterNo cnt, char *buffer);
	virtual int writeCluster(ClusterNo cnt, const char *buffer);
};

