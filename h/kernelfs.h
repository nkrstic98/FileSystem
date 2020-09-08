#pragma once

#include "fs.h"
#include "dataRep.h"

const unsigned int DIRNUM = ClusterSize / 32;

class KernelFS {
public:
	KernelFS();
	~KernelFS();

	char mount(Partition* partition); 
	char unmount();
	char format(); 
	FileCnt readRootDir();
	char doesExist(char* fname); 

	File* open(char* fname, char mode);
	char deleteFile(char* fname);

private:
	Partition* part;
	BitVector* bitVect;
	DirEntry** dirEntry;
};