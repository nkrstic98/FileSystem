#pragma once

typedef unsigned long ClusterNo;
const unsigned long ClusterSize = 2048;

class PartitionImpl;

class Partition {
public:
	Partition(char *partition);
	virtual ClusterNo getNumOfClusters() const; //vraca broj klastera koji pripadaju particiji

	virtual int readCluster(ClusterNo cnt, char *buffer); //cita zadati klaster i u slucaju uspeha vraca 1; u suprotnom 0
	virtual int writeCluster(ClusterNo cnt, const char *buffer); //upisuje zadati klaster i u slucaju uspeha vraca 1; u suprotnom 0

	virtual ~Partition();
private:
	PartitionImpl *myImpl;
};
