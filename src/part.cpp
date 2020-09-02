#include "part.h"
#include "kernelpart.h"

Partition::Partition(char *partition)
{
	myImpl = new PartitionImpl(partition);
}

ClusterNo Partition::getNumOfClusters() const
{
	return myImpl->getNumOfClusters();
}

int Partition::readCluster(ClusterNo cnt, char * buffer)
{
	return myImpl->readCluster(cnt, buffer);
}

int Partition::writeCluster(ClusterNo cnt, const char * buffer)
{
	return myImpl->writeCluster(cnt, buffer);
}

Partition::~Partition()
{
	delete myImpl;
}
