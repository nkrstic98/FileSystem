#include "kernelfs.h"
#include "kernelfile.h"
#include <iostream>

using namespace std;

KernelFS::KernelFS() : nr(0), nw(0), dr(0), dw(0) {
	part = nullptr;
	openFiles = new FileList();

	mutex = CreateSemaphore(0, 1, 1, NULL);
	mountSem = CreateSemaphore(0, 0, 1, NULL);
	unmountSem = CreateSemaphore(0, 0, 1, NULL);

	r = CreateSemaphore(0, 0, 1, NULL);
	w = CreateSemaphore(0, 0, 1, NULL);
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
	if (partition == nullptr) { return 0; }

	wait(mutex);

	if (part != nullptr) {
		signal(mutex); //ako se blokiram, treba da oslobodim ekskluzivni pristup
		wait(mountSem); //blokiram se dok se ne unmountuje particija
		wait(mutex); //cekam dok ne dobijem ekskluzivni pristup
	}

	part = partition;

	directoryCluster = new char[ClusterSize];
	bitVectorCluster = new char[ClusterSize];

	part->readCluster(0, (char*)bitVectorCluster); //montiranje bit vektora
	part->readCluster(1, (char*)directoryCluster); //montiranje direktorijuma

	bitVect = new BitVector(bitVectorCluster, part->getNumOfClusters());
	dirEntry = new DirCluster(directoryCluster);

	signal(mutex); //oslobadjam pristup deljenim promenljivama

	return 1;
}

char KernelFS::unmount()
{
	wait(mutex);

	if (part == nullptr) { signal(mutex); return 0; }

	if (!openFiles->empty()) {
		signal(mutex);
		wait(unmountSem);
		wait(mutex);
	}

	

	delete part;

	part = nullptr;

	delete[]bitVectorCluster;
	delete[]directoryCluster;

	delete bitVect;
	delete dirEntry;

	bitVect = nullptr;
	dirEntry = nullptr;

	signal(mountSem);
	signal(mutex);
}

char KernelFS::format()
{
	wait(mutex);

	if (part == nullptr) { signal(mutex); return 0; }

	bitVect->format();

	part->writeCluster(0, bitVect->bitVect);

	char clear[ClusterSize] = { 0 };

	for (int i = 1; i < part->getNumOfClusters(); i++) { //formatiranje svih klastera, ukljucujuci i root direktorijuma
		part->writeCluster(i, (char*)clear);
	}

	part->readCluster(1, directoryCluster); //procitamo novu formatiranu vrednost klastera direktorijuma

	signal(mutex);

	return 1;
}

FileCnt KernelFS::readRootDir()
{
	wait(mutex);

	if (part == nullptr) {
		signal(mutex);
		return -1;
	}

	signal(mutex);

	return dirEntry->getFileNum();
}

char KernelFS::doesExist(char * fname)
{
	wait(mutex);

	if (part == nullptr) {
		signal(mutex);
		return -1;
	}

	signal(mutex);

	return dirEntry->fileExists(fname);
}

File * KernelFS::open(char * fname, char mode)
{
	File *f;

	wait(mutex);

	if (part == nullptr) {
		signal(mutex);
		return nullptr;
	}

	char fileName[13] = { 0 };
	int i = 1;
	for (; i < strlen(fname); i++) fileName[i - 1] = fname[i];
	fileName[i] = '\0';

	bool exists = dirEntry->fileExists(fileName) == 1 ? true : false;
	bool open = exists && (KernelFS::openFiles->isOpen(fileName) != nullptr);

	//fajl ne postoji na disku
	if (!exists) {
		if (mode == 'r' || mode == 'a') {
			signal(mutex);
			return nullptr;
		}
		if (mode == 'w') {
			f =  openForWrite(fileName, false);
			nw++;
			signal(mutex);
			return f;
		}
	}

	if (exists && !open) {
		switch (mode)
		{
		case 'r':
			f = openForRead(fileName);
			nr++;
			signal(mutex);
			return f;
			break;

		case 'w':
			//formatiraj fajl sa zadatim imenom
			formatFile(fileName);
			f = openForWrite(fileName, true);
			nw++;
			signal(mutex);
			return f;
			break;

		case 'a':
			f = openForAppend(fileName);
			nw++;
			signal(mutex);
			return f;
			break;
		}
	}

	if(open)
	{
		/*
		switch (openFiles->isOpen(fileName)->getKernelFile()->getMode())
		{
		case 'r':
			if (mode == 'r') return openForRead(fileName);
			if (mode == 'w') {
				signal(mutex);
				
				wait(mutex);
				
				if (KernelFS::doesExist(fileName) == 1) {
					this->formatFile(fname);
					return this->openForWrite(fname, true);
				}
				else {
					signal(mutex);
					return this->openForWrite(fname, false);
				}
			}
			if (mode == 'a') {
				while (openFiles->isOpen(fileName)) { blokiraj se dok god je fajl otvoren }
				if (KernelFS::doesExist(fileName) == 1) {
					return this->openForAppend(fname);
				}
				else {
					signal(mutex);
					return nullptr;
				}
			}
			break;

		case 'w':
			while (openFiles->isOpen(fileName)) { blokiraj se dok god je fajl otvoren }

			if (mode == 'r') 
				if (KernelFS::doesExist(fileName) == 1) return openForRead(fileName); 
				else { signal(mutex); return nullptr; }

			if (mode == 'w') {
				if (KernelFS::doesExist(fileName) == 1) {
					this->formatFile(fname);
					return this->openForWrite(fname, true);
				}
				else {
					signal(mutex);
					return this->openForWrite(fname, false);
				}
			}

			if (mode == 'a')
				if (KernelFS::doesExist(fileName) == 1) return openForAppend(fileName);
				else { signal(mutex); return nullptr; }

			break;

		case 'a':
			while (openFiles->isOpen(fileName)) { blokiraj se dok god je fajl otvoren }

			if (mode == 'r')
				if (KernelFS::doesExist(fileName) == 1) return openForRead(fileName);
				else { signal(mutex); return nullptr; }

			if (mode == 'w') {
				if (KernelFS::doesExist(fileName) == 1) {
					this->formatFile(fname);
					return this->openForWrite(fname, true);
				}
				else {
					signal(mutex);
					return this->openForWrite(fname, false);
				}
			}

			if (mode == 'a')
				if (KernelFS::doesExist(fileName) == 1) return openForAppend(fileName);
				else { signal(mutex); return nullptr; }
			break;
		}
		*/

		switch (mode)
		{
		case 'r':
			if (nw > 0) {
				dr++;
				signal(mutex);
				wait(r);
				wait(mutex);
			}
			nr++;
			f = openForRead(fileName);
			if (dr > 0) {
				dr--;
				signal(r);
			}
			signal(mutex);
			return f;
			break;

		case 'w':
		case 'a':
			if (nw > 0 || nr > 0) {
				dw++;
				signal(mutex);
				wait(w);
				wait(mutex);
			}
			nw++;
			if (mode == 'w') {
				if (dirEntry->fileExists(fileName) == 1) {
					this->formatFile(fileName);
					f =  this->openForWrite(fileName, true);
				}
				else {
					f =  this->openForWrite(fileName, false);
				}
			}
			if (mode == 'a') {
				if (dirEntry->fileExists(fileName) == 1) {
					this->formatFile(fileName);
					f = this->openForAppend(fileName);
				}
				else {
					f = nullptr;
				}
			}
			signal(mutex);
			return f;
			break;
		}
	}
}

File* KernelFS::openForRead(char* fname)
{
	File *file = new File();
	file->getKernelFile()->setMyEntry(dirEntry->getMyEntry(fname));
	file->getKernelFile()->setMode('r');

	uint32_t index1Cluster = dirEntry->getCluster(dirEntry->getMyEntry(fname));
	char *index1 = new char[ClusterSize];
	part->readCluster(index1Cluster, index1); //ucitaj vrednost u klaster prvog nivoa
	file->getKernelFile()->setIndexCluster(index1, index1Cluster);

	file->getKernelFile()->open = true;

	file->seek(0);

	openFiles->add(file);

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
	file->seek(0);
	file->getKernelFile()->empty = true;
	file->getKernelFile()->open = true;

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
	file->getKernelFile()->setIndexCluster(index1, index1Cluster);

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

	openFiles->add(file);

	dirEntry->setName(entry, fname);
	dirEntry->setCluster(entry, index1Cluster);
	dirEntry->setSize(entry, 0); //na pocetku je velicina fajla jednaka 0

	part->writeCluster(0, bitVectorCluster);
	part->writeCluster(1, directoryCluster);

	return file;
}

File* KernelFS::openForAppend(char* fname)
{
	File *file = new File();
	file->getKernelFile()->setMyEntry(dirEntry->getMyEntry(fname));
	file->getKernelFile()->setMode('a');
	file->getKernelFile()->open = true;

	uint32_t index1Cluster = dirEntry->getCluster(dirEntry->getMyEntry(fname));
	char *index1 = new char[ClusterSize];
	part->readCluster(index1Cluster, index1); //ucitaj vrednost u klaster prvog nivoa
	file->getKernelFile()->setIndexCluster(index1, index1Cluster);

	file->seek(dirEntry->getSize(dirEntry->getMyEntry(fname)));

	openFiles->add(file);

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
	wait(mutex);

	if (part == nullptr) {
		signal(mutex); 
		return 0;
	}

	char fileName[13] = { 0 };
	int i = 1;
	for (; i < strlen(fname); i++) fileName[i - 1] = fname[i];
	fileName[i] = '\0';

	bool exists = doesExist(fileName) == 1 ? true : false;
	if (!exists) {
		signal(mutex);
		return 0;
	}

	formatFile(fileName);

	int cluster = dirEntry->getCluster(dirEntry->getMyEntry(fileName));

	char format[ClusterSize] = { 0 };
	bitVect->freeCluster(cluster);

	part->writeCluster(cluster, (char*)format);

	DirEntry de;

	DirEntry *dEntry = dirEntry->getEntry();

	dEntry[dirEntry->getMyEntry(fileName)] = de;

	part->writeCluster(0, bitVectorCluster);
	part->writeCluster(1, directoryCluster);

	signal(mutex);

	return 1;
}
