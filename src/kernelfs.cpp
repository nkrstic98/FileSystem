#include "kernelfs.h"
#include "kernelfile.h"
#include <iostream>

using namespace std;

KernelFS::KernelFS() : part(nullptr) {
	openFiles = new FileList();
}

KernelFS::~KernelFS()
{
	unmount();
	delete part;
	delete bitVect;
	delete dirEntry;
}

char KernelFS::mount(Partition * partition)
{
	if (partition == nullptr) return 0;

	if (this->part != nullptr) {
		//ovde ide kod ako neko pokusa da montira dok postoji montirana particija
	}

	this->part = partition;

	this->directoryCluster = new char[ClusterSize];
	this->bitVectorCluster = new char[ClusterSize];

	part->readCluster(0, (char*)bitVectorCluster); //montiranje bit vektora
	part->readCluster(1, (char*)directoryCluster); //montiranje direktorijuma

	bitVect = new BitVector(bitVectorCluster, part->getNumOfClusters());
	dirEntry = new DirCluster(directoryCluster);

	return 1;
}

char KernelFS::unmount()
{
	if (part == nullptr) return 0;

	if (!openFiles->empty()) {
		//ako ima otvorenih fajlova blokiramo se dok se lista ne isprazni, to jest svi fajlovi zatvore
	}

	delete part;

	part = nullptr;

	delete[]bitVectorCluster;
	delete[]directoryCluster;

	delete bitVect;
	delete dirEntry;

	bitVect = nullptr;
	dirEntry = nullptr;
}

char KernelFS::format()
{
	if (part == nullptr) return 0;

	bitVect->format();

	part->writeCluster(0, bitVect->bitVect);

	char clear[ClusterSize] = { 0 };

	for (int i = 1; i < part->getNumOfClusters(); i++) { //formatiranje svih klastera, ukljucujuci i root direktorijuma
		part->writeCluster(i, (char*)clear);
	}

	part->readCluster(1, directoryCluster); //procitamo novu formatiranu vrednost klastera direktorijuma

	return 1;
}

FileCnt KernelFS::readRootDir()
{
	if (part == nullptr) return -1;

	return dirEntry->getFileNum();
}

char KernelFS::doesExist(char * fname)
{
	if (part == nullptr) return -1;

	return dirEntry->fileExists(fname);
}

File * KernelFS::open(char * fname, char mode)
{
	bool exists = this->doesExist(fname) == 1 ? true : false;
	bool open = exists && (this->openFiles->isOpen(fname) != nullptr);

	//fajl ne postoji na disku
	if (!exists) {
		if (mode == 'r' || mode == 'a') return nullptr;
		if (mode == 'w') {
			return openForWrite(fname, false);
		}
	}

	if (exists && !open) {
		switch (mode)
		{
		case 'r':
			return this->openForRead(fname);
			break;

		case 'w':
			//formatiraj fajl sa zadatim imenom
			this->formatFile(fname);
			return this->openForWrite(fname, true);
			break;

		case 'a':
			return this->openForAppend(fname);
			break;
		}
	}

	if(open)
	{
		switch (this->openFiles->isOpen(fname)->getKernelFile()->getMode())
		{
		case 'r':
			if (mode == 'r') return this->openForRead(fname);
			if (mode == 'w' || mode == 'a') { /*blokiraj se*/ }
			break;

		case 'w':
			//blokiraj se
			break;

		case 'a':
			//blokiraj se
			break;
		}
	}
}

File* KernelFS::openForRead(char* fname)
{
	File *file = new File();
	file->getKernelFile()->setMyEntry(dirEntry->getMyEntry(fname));
	file->getKernelFile()->setMode('r');
	file->getKernelFile()->dirCluster = this->dirEntry;

	file->seek(0);

	this->openFiles->add(file);

	return file;
}

File* KernelFS::openForWrite(char* fname, bool formated)
{
	int entry;
	if (!formated) entry = dirEntry->findFreeEntry();
	else
		entry = dirEntry->getMyEntry(fname);

	if (entry == -1) return nullptr; //nema slobodnog mesta u direktorijumu

	if (bitVect->freeClustersNum() < 3) return nullptr; //nema dovoljno klastera za alokaciju novog fajla

	File *file = new File();
	file->getKernelFile()->setMyEntry(entry);
	file->getKernelFile()->setMode('w');
	file->getKernelFile()->dirCluster = this->dirEntry;

	char format[ClusterSize] = { 0 };

	uint32_t index1Cluster;
	char *index1 = new char[ClusterSize];
	if (!formated) {
		index1Cluster = bitVect->takeCluster(); //zauzmi klaster prvog nivoa
		part->writeCluster(index1Cluster, format); //upisi sve 0 u klaster prvog nivoa
	}
	else {
		index1Cluster = dirEntry->getCluster(dirEntry->getMyEntry(fname));
	}
	part->readCluster(index1Cluster, index1); //ucitaj vrednost u klaster prvog nivoa
	file->getKernelFile()->setIndexCluster(index1);

	uint32_t index2Cluster = bitVect->takeCluster(); //zauzmi klaster drugog nivoa
	char *index2 = new char[ClusterSize];
	part->writeCluster(index2Cluster, format); //formatiraj ga
	part->readCluster(index2Cluster, index2); //iscitaj iz fajla
	file->getKernelFile()->getIndexCluster()->setEntry(index2Cluster); //poistavi ulaz u indeks prvog nivoa
	part->writeCluster(index1Cluster, index1); //upisi sada novu vrednost u klaster indeksa prvog nivoa

	IndexEntry *level2 = (IndexEntry*)index2;

	uint32_t dataCluster = bitVect->takeCluster(); //alociraj klaster za podatke
	part->writeCluster(dataCluster, format); //formatiraj ga
	level2->entries[0] = dataCluster; //prvi ulaz indeksa drugog nivoa dobija vrednost data klastera
	part->writeCluster(index2Cluster, index2); //sacuvamo to sto smo upisali

	this->openFiles->add(file);

	this->dirEntry->setName(entry, fname);
	this->dirEntry->setCluster(entry, index1Cluster);
	this->dirEntry->setSize(entry, 0); //na pocetku je velicina fajla jednaka 0

	part->writeCluster(0, bitVectorCluster);
	part->writeCluster(1, directoryCluster);

	return file;
}

File* KernelFS::openForAppend(char* fname)
{
	File *file = new File();
	file->getKernelFile()->setMyEntry(dirEntry->getMyEntry(fname));
	file->getKernelFile()->setMode('a');
	file->getKernelFile()->dirCluster = this->dirEntry;

	file->seek(this->dirEntry->getSize(dirEntry->getMyEntry(fname)));

	this->openFiles->add(file);

	return file;
}

void KernelFS::formatFile(char * fname)
{
	char index1[ClusterSize];
	
	int entry = dirEntry->getMyEntry(fname);
	int cluster = dirEntry->getCluster(entry);

	part->readCluster(cluster, index1);

	IndexEntry* l1Entry = (IndexEntry*)index1;

	int cluster2, dataCluster;
	char index2[ClusterSize], format[ClusterSize] = { 0 };
	IndexEntry *l2Entry = (IndexEntry*)index2;

	for (int i = 0; i < INDEXSIZE; i++) {
		if (l1Entry->entries[i] == 0) break;

		cluster2 = l1Entry->entries[i];

		part->readCluster(cluster2, index2);

		for (int j = 0; j < INDEXSIZE; j++) {
			if (l2Entry->entries[j] == 0) break;

			dataCluster = l2Entry->entries[j];

			part->writeCluster(dataCluster, (char*)format);
			bitVect->freeCluster(dataCluster);
		}

		part->writeCluster(cluster2, (char*)format);
		bitVect->freeCluster(cluster2);

		l1Entry->entries[i] = 0;
	}

	part->writeCluster(0, bitVectorCluster);
	part->writeCluster(cluster, format);
}

char KernelFS::deleteFile(char * fname)
{
	return 0;
}
