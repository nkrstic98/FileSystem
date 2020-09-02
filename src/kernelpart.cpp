#include "kernelpart.h"

PartitionImpl::PartitionImpl(char *partition)
{

}

PartitionImpl::~PartitionImpl()
{

}

ClusterNo PartitionImpl::getNumOfClusters() const
{
	return ClusterNo();
}

int PartitionImpl::readCluster(ClusterNo cnt, char * buffer)
{
	return 0;
}

int PartitionImpl::writeCluster(ClusterNo cnt, const char * buffer)
{
	return 0;
}
