#include "kernelfs.h"
#include "part.h"

#include <iostream>

using namespace std;

KernelFS::KernelFS() : part(nullptr) {}

KernelFS::~KernelFS() {}

char KernelFS::mount(Partition * partition)
{
	if (partition == nullptr || this->part != nullptr) return 0;

	this->part = partition;

	bitVect = new BitVector();

	part->readCluster(0, (char*)bitVect->vector);

	dirEntry = new DirEntry*[DIRNUM];

	char *rootCluster = new char[ClusterSize];

	part->readCluster(1, (char*)rootCluster);

	for (int i = 0; i < DIRNUM; i++) {
		dirEntry[i] = new DirEntry();
		dirEntry[i] = (DirEntry*)rootCluster[i * 32];
	}

	return 1;
}

char KernelFS::unmount()
{
	if (part == nullptr) return 0;

	this->part = nullptr;

	delete bitVect;
	this->bitVect = nullptr;

	for (int i = 0; i < DIRNUM; i++)
		delete this->dirEntry[i];

	delete[]dirEntry;

	dirEntry = nullptr;
}

char KernelFS::format()
{
	if (part == nullptr) return 0;



	return 1;
}

FileCnt KernelFS::readRootDir()
{
	return FileCnt();
}

char KernelFS::doesExist(char * fname)
{
	return 0;
}

File * KernelFS::open(char * fname, char mode)
{
	return nullptr;
}

char KernelFS::deleteFile(char * fname)
{
	return 0;
}
