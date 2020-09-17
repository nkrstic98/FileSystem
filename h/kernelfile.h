#pragma once

#include "dataRep.h"

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

	void setName(char *fname);
	char* getName() const;

	void setMode(char m);
	char getMode() const;

private:
	char fname[13];
	char mode;
};