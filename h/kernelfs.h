#pragma once

#include "fs.h"
#include "dataRep.h"

#define SetBit(A,k)     ( A[(k / 8)] |= (1 << (k % 8)) )
#define ClearBit(A,k)   ( A[(k / 8)] &= ~(1 << (k % 8)) )
#define TestBit(A,k)    ( A[(k / 8)] & (1 << (k % 8)) )

const unsigned int DIRNUM = ClusterSize / 32;
const unsigned int BITNUM = ClusterSize * 8;

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

	void mountRootDir();

private:
	Partition* part;
	BitVector* bitVect;
	DirEntry* dirEntry;
};