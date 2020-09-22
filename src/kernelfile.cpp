#include "kernelfile.h"
#include "kernelFS.h"
#include <iostream>

KernelFile::KernelFile() : posPtr(0) {}

KernelFile::~KernelFile() 
{
	delete indexCluster;
	FS::getKernelFS()->openFiles->remove(this);
	open = false;


	wait(FS::getKernelFS()->mutex);
	switch (this->mode)
	{
	case 'r':
		FS::getKernelFS()->nr--;
		if (FS::getKernelFS()->nr == 0 && FS::getKernelFS()->dw > 0) {
			FS::getKernelFS()->dw--;
			signal(FS::getKernelFS()->w);
		}
		break;

	case 'w':
	case 'a':
		if (FS::getKernelFS()->dr > 0) {
			FS::getKernelFS()->dr--;
			signal(FS::getKernelFS()->r);
		}
		else if (FS::getKernelFS()->dw > 0) {
			FS::getKernelFS()->dw--;
			signal(FS::getKernelFS()->w);
		}
		break;
	}
	signal(FS::getKernelFS()->mutex);

	if (FS::getKernelFS()->openFiles->empty()) signal(FS::getKernelFS()->unmountSem);
}

char KernelFile::write(BytesCnt cnt, char * buffer)
{
	if (mode == 'r') return 0;

	if (getFileSize() == MAXSIZE) return 0;
	if (FS::getKernelFS()->bitVect->freeClustersNum() == 0) return 0;
	if (cnt == 0) return 0;

	int start, index2, index1, remainingSpace;
	long long int i = 0, fileSize = getFileSize();
	int newCluster, newIndex;

	char l2Cluster[ClusterSize], dataCluster[ClusterSize], format[ClusterSize] = { 0 };
	IndexEntry *index2Cluster = (IndexEntry*)l2Cluster;

	for (int j = 0; j < cnt; j++) {
		start = posPtr % ClusterSize;
		index2 = posPtr / ClusterSize;
		index1 = index2 / 512;
		if (index2 >= INDEXSIZE) {
			if (index2 % 512 == 0) {
				index2 = 0;
			}
			else {
				index2 = index2 - index1 * 512;
			}
		}

		FS::getKernelFS()->part->readCluster(indexCluster->getEntry(index1), l2Cluster);

		FS::getKernelFS()->part->readCluster(index2Cluster->entries[index2], dataCluster);

		dataCluster[start] = buffer[i++];

		FS::getKernelFS()->part->writeCluster(index2Cluster->entries[index2], dataCluster);

		posPtr++; fileSize++;

		if(start == ClusterSize - 1) {

			if (FS::getKernelFS()->bitVect->freeClustersNum() < 2 || index1 == INDEXSIZE - 1)
				if (fileSize > 0) return 1;
				else return 0;

			if (index2 == INDEXSIZE - 1) {
				newIndex = FS::getKernelFS()->bitVect->takeCluster();

				indexCluster->setEntry(newIndex);
				FS::getKernelFS()->part->writeCluster(indexCluster->clusterNum, (char*)indexCluster->index_1->entries);

				index1++;
				FS::getKernelFS()->part->readCluster(indexCluster->getEntry(index1), l2Cluster);

				index2 = 0;
			}
			else
				index2++;

			newCluster = FS::getKernelFS()->bitVect->takeCluster();
			FS::getKernelFS()->part->writeCluster(newCluster, format);
			index2Cluster->entries[index2] = newCluster;
			FS::getKernelFS()->part->writeCluster(indexCluster->getEntry(index1), l2Cluster);
		}

		if (j == cnt - 1) {
			FS::getKernelFS()->dirEntry->setSize(myEntry, fileSize);
			FS::getKernelFS()->part->writeCluster(1, FS::getKernelFS()->directoryCluster);
			return 1;
		}
	}

	return 1;
}

BytesCnt KernelFile::read(BytesCnt cnt, char * buffer)
{
	if (mode == 'a' || mode == 'w') return 0;

	if (!open) return 0;
	if (eof()) return 0;

	int start = posPtr % ClusterSize;
	int index2 = posPtr / ClusterSize;
	int index1 = index2 / 512;
	if (index2 >= INDEXSIZE) {
		if (index2 % 512 == 0) {
			index2 = 0;
		}
		else {
			index2 = index2 - index1 * 512;
		}
	}

	long long int ind = 0;

	char l2Cluster[ClusterSize], dataCluster[ClusterSize];
	IndexEntry *index2Cluster = (IndexEntry*)l2Cluster;

	for (; index1 < INDEXSIZE; index1++) {
		if (indexCluster->getEntry(index1) == 0) break;

		FS::getKernelFS()->part->readCluster(indexCluster->getEntry(index1), l2Cluster);

		for (; index2 < INDEXSIZE; index2++) {
			if (index2Cluster->entries[index2] == 0) break;

			FS::getKernelFS()->part->readCluster(index2Cluster->entries[index2], dataCluster);

			for (; start < ClusterSize; start++) {
				buffer[ind++] = dataCluster[start];
				posPtr++;
				if (ind == cnt) return ind;
			}

			start = 0;
		}

		index2 = 0;
	}

	return ind;
}

char KernelFile::seek(BytesCnt cnt)
{
	if (getFileSize() < cnt) return 0;
	this->posPtr = cnt;
	return 1;
}

BytesCnt KernelFile::filePos()
{
	return posPtr;
}

char KernelFile::eof()
{
	if (posPtr == getFileSize()) return 1;
	return 0;
}

BytesCnt KernelFile::getFileSize()
{
	return FS::getKernelFS()->dirEntry->getSize(myEntry);
}

char KernelFile::truncate()
{
	if (mode == 'r') return 0;

	long long int fsize = posPtr;

	int index2 = ceil((double)posPtr / 2048.0) - 1;
	if (index2 < 0) index2 = 0;
	int index1 = ceil((double)index2 / 512.0) - 1;
	if (index1 < 0) index1 = 0;

	char l2Cluster[ClusterSize];
	char dataCluster[ClusterSize];

	char format[ClusterSize] = { 0 };

	IndexEntry *index2Cluster = (IndexEntry*)l2Cluster;

	bool iteration1 = true;

	for (int i = index1; i < INDEXSIZE; i++) {
		if (indexCluster->getEntry(i) == 0) break;

		FS::getKernelFS()->part->readCluster(indexCluster->getEntry(i), l2Cluster);

		int j;
		if (iteration1) {
			j = index2;
		}
		else 
			j = 0;

		for (; j < INDEXSIZE; j++) {
			if (index2Cluster->entries[j] == 0) break;

			FS::getKernelFS()->part->readCluster(index2Cluster->entries[j], dataCluster);

			int start;
			if (iteration1) {
				iteration1 = false;
				start = posPtr - (index2 * ClusterSize);
				for (; start < ClusterSize; start++) dataCluster[start] = 0;
				FS::getKernelFS()->part->writeCluster(index2Cluster->entries[j], dataCluster);
			}
			else {
				FS::getKernelFS()->part->writeCluster(index2Cluster->entries[j], format);
				FS::getKernelFS()->bitVect->freeCluster(index2Cluster->entries[j]);
				index2Cluster->entries[j] = 0;
			}
		}

		FS::getKernelFS()->part->writeCluster(indexCluster->getEntry(i), l2Cluster);

		if (index2Cluster->entries[0] == 0) {
			FS::getKernelFS()->part->writeCluster(indexCluster->getEntry(i), format);
			FS::getKernelFS()->bitVect->freeCluster(indexCluster->getEntry(i));
			indexCluster->index_1->entries[i] = 0;
		}
	}

	FS::getKernelFS()->part->writeCluster(myEntry, (char*)indexCluster->index_1->entries);

	FS::getKernelFS()->dirEntry->setSize(myEntry, fsize);
	FS::getKernelFS()->part->writeCluster(1, FS::getKernelFS()->directoryCluster);

	return 1;
}

void KernelFile::setMode(char m)
{
	mode = m;
}

char KernelFile::getMode() const
{
	return mode;
}

void KernelFile::setMyEntry(int e)
{
	myEntry = e;
}

int KernelFile::getMyEntry() const
{
	return myEntry;
}

void KernelFile::setIndexCluster(char * cluster, uint32_t entry)
{
	this->indexCluster = new IndexCluster(cluster, entry);
}

IndexCluster * KernelFile::getIndexCluster() const
{
	return indexCluster;
}
