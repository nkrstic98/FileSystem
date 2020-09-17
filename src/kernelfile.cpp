#include "kernelfile.h"

KernelFile::KernelFile() {}
KernelFile::~KernelFile() {}

char KernelFile::write(BytesCnt, char * buffer)
{
	return 0;
}

BytesCnt KernelFile::read(BytesCnt, char * buffer)
{
	return BytesCnt();
}

char KernelFile::seek(BytesCnt)
{
	return 0;
}

BytesCnt KernelFile::filePos()
{
	return BytesCnt();
}

char KernelFile::eof()
{
	return 0;
}

BytesCnt KernelFile::getFileSize()
{
	return BytesCnt();
}

char KernelFile::truncate()
{
	return 0;
}

void KernelFile::setMode(char m)
{
	mode = m;
}

char KernelFile::getMode() const
{
	return mode;
}

void KernelFile::setMyEntry(int e)
{
	myEntry = e;
}

int KernelFile::getMyEntry() const
{
	return myEntry;
}

void KernelFile::setIndexCluster(char * cluster)
{
	this->indexCluster = new IndexCluster(cluster);
}

IndexCluster * KernelFile::getIndexCluster() const
{
	return indexCluster;
}
