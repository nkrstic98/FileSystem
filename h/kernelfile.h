#pragma once

#include "dataRep.h"
#include "dirCluster.h"
#include "indexCluster.h"

class KernelFile
{
public:
	KernelFile();
	~KernelFile();

	char write(BytesCnt cnt, char* buffer);
	BytesCnt read(BytesCnt cnt, char* buffer);
	char seek(BytesCnt cnt);
	BytesCnt filePos();
	char eof();
	BytesCnt getFileSize();
	char truncate();

	void setMode(char m);
	char getMode() const;

	void setMyEntry(int e);
	int getMyEntry() const;

	void setIndexCluster(char* cluster, uint32_t entry);
	IndexCluster* getIndexCluster() const;

	bool open, empty;
	long long int posPtr; //pokazivac na tekucu poziciju u fajlu

private:
	int myEntry;
	char mode;
	IndexCluster *indexCluster;
};