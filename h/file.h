#pragma once

#include "fs.h"

class KernelFile;

class File {
public:
	~File(); //zatvaranje fajla
	char write(BytesCnt cnt, char* buffer);
	BytesCnt read(BytesCnt cnt, char* buffer);
	char seek(BytesCnt cnt);
	BytesCnt filePos();
	char eof();
	BytesCnt getFileSize();
	char truncate();

	KernelFile* getKernelFile() const;

	void setName(char *fname);
	char* getName() const;

private:
	friend class FS;
	friend class KernelFS;
	File(); //objekat fajla se može kreirati samo otvaranjem
	KernelFile *myImpl;
};