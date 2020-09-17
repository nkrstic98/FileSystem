#pragma once

#include "fs.h"
#include "dataRep.h"
#include "dirCluster.h"
#include "bitVector.h"
#include "fileList.h"

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

	File* openForRead(char* fname);
	File* openForWrite(char* fname, bool formated);
	File* openForAppend(char *fname);

	void formatFile(char *fname);

	char deleteFile(char* fname);

private:
	Partition* part;
	char *directoryCluster;
	char *bitVectorCluster;

	BitVector* bitVect;
	DirCluster* dirEntry;

	FileList* openFiles;
};
