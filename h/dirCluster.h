#pragma once

#include "dataRep.h"

struct DirEntry {
	char fname[FNAMELEN] = { 0 };
	char fext[FEXTLEN] = { 0 };
	char notUsed = 0;
	uint32_t cluster;
	uint32_t fileSize;
	char free[12] = { 0 };
};

class DirCluster
{
public:
	DirCluster(char* cluster);
	~DirCluster();

	int findFreeEntry();

	void setName(int entry, char* fullName);
	char* getName(int entry) const;
	
	void setCluster(int entry, int cluster);
	int getCluster(int entry) const;

	void setSize(int entry, int fSize);
	int getSize(char *fname) const;

	FileCnt getFileNum() const;

	char fileExists(char* fname) const;

	DirEntry *dirEntry;
};

