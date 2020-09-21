#include "dirCluster.h"
#include "fs.h"
#include "kernelfs.h"

DirCluster::DirCluster(char * cluster)
{
	dirEntry = (DirEntry*)cluster;
}

DirCluster::~DirCluster() {}

int DirCluster::findFreeEntry()
{
	for (int i = 0; i < DIRNUM; i++) {
		if (dirEntry[i].fname[0] == 0) return i;
	}

	return -1;
}

void DirCluster::setName(int entry, char* fullName)
{
	int i = 0;
	char fname[9] = { 0 };
	char fext[4] = { 0 };

	for (; i < strlen(fullName); i++) {
		if (fullName[i] == '.') break;
		fname[i] = fullName[i];
	}

	strcpy(dirEntry[entry].fname, fname);

	i++;
	int j = 0;
	for (; i < strlen(fullName); i++) fext[j++] = fullName[i];

	strcpy(dirEntry[entry].fext, fext);
}

char * DirCluster::getName(int entry) const
{
	if (dirEntry[entry].fname[0] == 0) return 0;

	char fullName[13] = { 0 };

	char fname[9] = { 0 };
	char fext[4] = { 0 };

	for (int i = 0; i < FNAMELEN; i++) fname[i] = dirEntry[entry].fname[i];
	for (int i = 0; i < FEXTLEN; i++) fext[i] = dirEntry[entry].fext[i];

	strcat(fullName, fname);
	strcat(fullName, ".");
	strcat(fullName, fext);

	return fullName;
}

void DirCluster::setCluster(int entry, int cluster)
{
	dirEntry[entry].cluster = cluster;
}

int DirCluster::getCluster(int entry) const
{
	return dirEntry[entry].cluster;
}

void DirCluster::setSize(int entry, int fSize)
{
	dirEntry[entry].fileSize = fSize;
}

int DirCluster::getSize(int entry) const
{
	return dirEntry[entry].fileSize;
}

FileCnt DirCluster::getFileNum() const
{
	int fileNum = 0;

	for (int i = 0; i < DIRNUM; i++) {
		if (dirEntry[i].fname[0] != 0) fileNum++;
	}

	return fileNum;
}

char DirCluster::fileExists(char * fname) const
{
	for (int i = 0; i < DIRNUM; i++) {
		char *name = this->getName(i);
		if (name == 0) continue;
		if (strcmp(fname, name) == 0) return 1;
	}

	return 0;
}

DirEntry * DirCluster::getEntry() const
{
	return dirEntry;
}

int DirCluster::getMyEntry(char * fname) const
{
	int i = 0;
	for (; i < DIRNUM; i++) {
		if (strcmp(this->getName(i), fname) == 0) return i;
	}

	return -1;
}


