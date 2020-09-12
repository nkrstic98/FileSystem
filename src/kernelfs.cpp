#include "kernelfs.h"
#include "part.h"

#include <iostream>
#include <string>

using namespace std;

KernelFS::KernelFS() : part(nullptr), fileNum(0) {}

KernelFS::~KernelFS() {}

char KernelFS::mount(Partition * partition)
{
	if (partition == nullptr) return 0;

	if (this->part != nullptr) {
		//ovde ide kod ako neko pokusa da montira dok postoji montirana particija
	}

	this->part = partition;

	bitVect = new BitVector();

	part->readCluster(0, (char*)bitVect->vector); //montiranje bit vectora

	mountRootDir(); //montiranje root direktorijuma

	return 1;
}

char KernelFS::unmount()
{
	if (part == nullptr) return 0;

	this->part = nullptr;

	delete bitVect;
	this->bitVect = nullptr;

	delete[]dirEntry;

	dirEntry = nullptr;
}

char KernelFS::format()
{
	if (part == nullptr) return 0;

	for (int i = 0; i < BITNUM; i++)
		SetBit(this->bitVect->vector, i); //postavi sve bitove na 1

	ClearBit(this->bitVect->vector, 0); //zauzmi bit 0 za BitVect klaster
	ClearBit(this->bitVect->vector, 1); //zauzmi bit 1 za klaster indeksa direktorijuma

	part->writeCluster(0, this->bitVect->vector); //zapamti formatiranje klastera 0

	//---------------------------------------------

	for (int i = 0; i < DIRNUM; i++) {
		dirEntry[i].fname[0] = 0;
	}

	this->fileNum = 0; //formatirana particija ne sadrzi fajlove

	char rootDir[ClusterSize] = { 0 };

	part->writeCluster(1, rootDir); //zapamti formatiranje klastera 1

	//Comment: ostale klastere ne treba formatirati, nakon formatiranja root direktorijuma smatra se da su svi slobodni

	return 1;
}

FileCnt KernelFS::readRootDir()
{
	if (part == nullptr) return -1;

	return this->fileNum;
}

char KernelFS::doesExist(char * fname)
{
	if (part == nullptr) return -1;

	if (this->fileNum == 0) return 0;

	for (int i = 0; i < DIRNUM; i++) {
		if (dirEntry[i].fname[0] == 0) continue; //ulaz je slobodan, ne sadrzi podatke o fajlu

		if (strcmp(dirEntry[i].fname, fname) == 0) return 1;
	}

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

void KernelFS::mountRootDir()
{
	dirEntry = new DirEntry[DIRNUM];

	char *rootCluster = new char[ClusterSize];

	part->readCluster(1, (char*)rootCluster);

	int hex0, hex1, hex2, hex3;

	for (int i = 0; i < DIRNUM; i++) {
		if (rootCluster[i * 32] == 0) { //ako je ulaz direktorijuma prazan, nema potrebe citati ostale podatke, vec samo zapamtiti da je ulaz prazan
			dirEntry[i].fname[0] = 0;
			continue;
		}

		char *hexval = new char[9];
		char *byte = new char[3];

		for (int j = 0; j < FNAMELEN; j++) dirEntry[i].fname[j] = rootCluster[i * 32 + j];
		for (int j = 0; j < FEXTLEN; j++) dirEntry[i].fext[j] = rootCluster[i * 32 + FNAMELEN + j];
	
		hex0 = rootCluster[i * 32 + FNAMELEN + FEXTLEN + 1];
		hex1 = rootCluster[i * 32 + FNAMELEN + FEXTLEN + 1 + 1];
		hex2 = rootCluster[i * 32 + FNAMELEN + FEXTLEN + 1 + 2];
		hex3 = rootCluster[i * 32 + FNAMELEN + FEXTLEN + 1 + 3];

		itoa(hex3, hexval, 16);
		strcat(hexval, itoa(hex2, byte, 16));
		strcat(hexval, itoa(hex1, byte, 16));
		strcat(hexval, itoa(hex0, byte, 16));

		dirEntry[i].cluster = stoi(hexval, 0, 16);

		hex0 = rootCluster[i * 32 + FNAMELEN + FEXTLEN + 1 + 4];
		hex1 = rootCluster[i * 32 + FNAMELEN + FEXTLEN + 1 + 4 + 1];
		hex2 = rootCluster[i * 32 + FNAMELEN + FEXTLEN + 1 + 4 + 2];
		hex3 = rootCluster[i * 32 + FNAMELEN + FEXTLEN + 1 + 4 + 3];

		if (hex0 < 0) hex0 += 256; //resava problem citanja iz memorije
		if (hex1 < 0) hex1 += 256;
		if (hex1 < 0) hex1 += 256;
		if (hex1 < 0) hex1 += 256;

		itoa(hex3, hexval, 16);
		strcat(hexval, itoa(hex2, byte, 16));
		strcat(hexval, itoa(hex1, byte, 16));
		strcat(hexval, itoa(hex0, byte, 16));

		uint64_t s = stoi(hexval, 0, 16);

		dirEntry[i].fileSize = stoi(hexval, 0, 16);

		this->fileNum++;

		delete[]hexval;
		delete[]byte;
	}
}
