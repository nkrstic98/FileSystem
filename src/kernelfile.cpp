#include "kernelfile.h"
#include "kernelFS.h"

KernelFile::KernelFile() : posPtr(0) {}

KernelFile::~KernelFile() 
{
	delete indexCluster;
	FS::getKernelFS()->openFiles->remove(this);
	open = false;
}

char KernelFile::write(BytesCnt cnt, char * buffer)
{
	if (mode == 'r') return 0;

	if (getFileSize() == MAXSIZE) return 0;
	if (FS::getKernelFS()->bitVect->freeClustersNum() == 0) return 0;
	if (cnt == 0) return 0;

	int index2 = ceil((double)posPtr / 2048.0) - 1;
	if (index2 < 0) index2 = 0;
	int index2_help = index2;
	int index1 = ceil((double)index2 / 512.0) - 1;
	if (index1 < 0) index1 = 0;

	char l2Cluster[ClusterSize];
	IndexEntry *index2Cluster = (IndexEntry*)l2Cluster;
	FS::getKernelFS()->part->readCluster(indexCluster->getEntry(index1), l2Cluster);

	for (int q = 0; q < INDEXSIZE; q++) {
		if (index2Cluster->entries[q] != 0) index2 = q;
		else break;
	}

	int neededClusters; //koliko mi max klastera treba
	bool useAllocated = false;
	int fileSize = getFileSize();

	if (posPtr % 2048 != 0) {
		int remainingSpace = 2048 - (posPtr - (index2_help * ClusterSize));
		neededClusters = ceil((double)(cnt - remainingSpace) / 2048.0);
		useAllocated = true;
		empty = false;
	}
	else {
		neededClusters = ceil((double)cnt / 2048.0); //ako mi je potreban ceo broj klastera
	}

	long long int i = 0; //ovo mi je pokazivac na pocetak fajla

	char dataCluster[ClusterSize] = { 0 };

	if (useAllocated) {
		FS::getKernelFS()->part->readCluster(index2Cluster->entries[index2], dataCluster);
		int start = posPtr - (index2_help * ClusterSize);
		for (; start < ClusterSize; start++) {
			dataCluster[start] = buffer[i++];
			posPtr++; fileSize++;
			if (i == cnt) {
				FS::getKernelFS()->part->writeCluster(index2Cluster->entries[index2], dataCluster);
				FS::getKernelFS()->dirEntry->setSize(myEntry, fileSize);
				FS::getKernelFS()->part->writeCluster(1, FS::getKernelFS()->directoryCluster);
				return 1;
			}
		}
		FS::getKernelFS()->part->writeCluster(index2Cluster->entries[index2], dataCluster);
	}

	int newCluster, newIndex;
	char format[ClusterSize] = { 0 };

	for (int j = 0; j < neededClusters; j++) {
		if (!empty) {
			index2++; //uzimamo prostor za novi klaster
			if (index2 == INDEXSIZE) {
				//alociramo novi klaster za indeks drugog nivoa
				if (FS::getKernelFS()->bitVect->freeClustersNum() < 2 || index1 == INDEXSIZE - 1)
					if (useAllocated) {
						FS::getKernelFS()->dirEntry->setSize(myEntry, fileSize);
						FS::getKernelFS()->part->writeCluster(1, FS::getKernelFS()->directoryCluster);
						return 1; //ako sam vec upisivao, vrati 1
					}
					else return 0; //inace, vrati 0

				newIndex = FS::getKernelFS()->bitVect->takeCluster(); //alociraj novi indeks

				indexCluster->setEntry(newIndex);
				index1++;
				FS::getKernelFS()->part->writeCluster(indexCluster->clusterNum, (char*)indexCluster->index_1->entries);

				FS::getKernelFS()->part->readCluster(indexCluster->getEntry(index1), l2Cluster);
				index2 = 0;
			}

			newCluster = FS::getKernelFS()->bitVect->takeCluster();
			if (newCluster == 0) {
				FS::getKernelFS()->dirEntry->setSize(myEntry, fileSize);
				FS::getKernelFS()->part->writeCluster(1, FS::getKernelFS()->directoryCluster);
				return 1; //ako nema vise slobodnih klastera
			}

			FS::getKernelFS()->part->writeCluster(newCluster, format);

			index2Cluster->entries[index2] = newCluster;
			FS::getKernelFS()->part->writeCluster(indexCluster->getEntry(index1), l2Cluster);

			for (int i = 0; i < ClusterSize; i++) dataCluster[i] = 0; //ocistimo niz svaki put kad krenemo da upisujemo u novi klaster
		}
		else {
			newCluster = index2Cluster->entries[index2];
		}

		for (int pos = 0; pos < ClusterSize; pos++) {
			dataCluster[pos] = buffer[i++];
			posPtr++; fileSize++;
			if (i == cnt) {
				empty = false;
				FS::getKernelFS()->part->writeCluster(index2Cluster->entries[index2], dataCluster);
				FS::getKernelFS()->dirEntry->setSize(myEntry, fileSize);
				FS::getKernelFS()->part->writeCluster(1, FS::getKernelFS()->directoryCluster);
				return 1;
			}
		}
	
		FS::getKernelFS()->part->writeCluster(newCluster, dataCluster);

		useAllocated = true;
		empty = false;
	}

	FS::getKernelFS()->dirEntry->setSize(myEntry, fileSize);
	FS::getKernelFS()->part->writeCluster(1, FS::getKernelFS()->directoryCluster);

	return 1;
}

BytesCnt KernelFile::read(BytesCnt cnt, char * buffer)
{
	if (mode == 'a' || mode == 'w') return 0;

	if (!open) return 0;
	if (eof()) return 0;

	int index2 = ceil((double)posPtr / 2048.0) - 1;
	if (index2 < 0) index2 = 0;
	int index1 = ceil((double)index2 / 512.0) - 1;
	if (index1 < 0) index1 = 0;

	int start = posPtr - (index2 * ClusterSize);
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
