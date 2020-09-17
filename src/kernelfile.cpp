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

void KernelFile::setName(char * fname)
{
	int i = 0;
	for (; i < strlen(fname); i++) this->fname[i] = fname[i];
	this->fname[i] = '\0';
}

char * KernelFile::getName() const
{
	return (char*)fname;
}

void KernelFile::setMode(char m)
{
	mode = m;
}

char KernelFile::getMode() const
{
	return mode;
}
