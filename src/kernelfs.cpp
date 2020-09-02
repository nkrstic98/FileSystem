#include "kernelfs.h"

KernelFS::KernelFS()
{
}

KernelFS::~KernelFS()
{
}

char KernelFS::mount(Partition * partition)
{
	return 0;
}

char KernelFS::unmount()
{
	return 0;
}

char KernelFS::format()
{
	return 0;
}

FileCnt KernelFS::readRootDir()
{
	return FileCnt();
}

char KernelFS::doesExist(char * fname)
{
	return 0;
}

File * KernelFS::open(char * fname, char mode)
{
	return nullptr;
}

char KernelFS::deleteFile(char * fname)
{
	return 0;
}
