#pragma once

#include "fs.h"

class KernelFS {
public:
	KernelFS();
	~KernelFS();

	static char mount(Partition* partition); 
	static char unmount();
	static char format(); 
	static FileCnt readRootDir();
	static char doesExist(char* fname); 

	static File* open(char* fname, char mode);
	static char deleteFile(char* fname);
};