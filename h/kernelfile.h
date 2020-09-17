#pragma once

#include "dataRep.h"
#include "dirCluster.h"
#include "indexCluster.h"

class KernelFile
{
public:
	KernelFile();
	~KernelFile();

	char write(BytesCnt, char* buffer);
	BytesCnt read(BytesCnt, char* buffer);
	char seek(BytesCnt);
	BytesCnt filePos();
	char eof();
	BytesCnt getFileSize();
	char truncate();

	void setMode(char m);
	char getMode() const;

	void setMyEntry(int e);
	int getMyEntry() const;

	void setIndexCluster(char* cluster);
	IndexCluster* getIndexCluster() const;

	DirCluster* dirCluster;
private:
	int myEntry;
	char mode;
	IndexCluster *indexCluster;
};