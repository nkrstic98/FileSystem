#include "kernelfs.h"
#include "kernelfile.h"

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
	//if (bitVect->freeClusters() < 3) return nullptr;

	bool exists = this->doesExist(fname) == 1 ? true : false;
	File *openFile = nullptr;

	if (exists) {
		openFile = this->openFiles->open(fname);
	}

	//fajl ne postoji na disku
	if (!exists) {
		if (mode == 'r' || mode == 'a') return nullptr;
		if (mode == 'w') {
			//kreiranje fajla
		}
	}

	//fajl postoji na disku, ali nije otvoren
	if (exists && openFile == nullptr) {
		switch (mode)
		{
		case 'r':
			openFile = new File(); //napravimo novi objekat fajla
			openFile->setName(fname); //sacuvamo naziv fajla
			openFile->getKernelFile()->setMode(mode); //sacuvamo mod u kom je otvoren
			openFile->seek(0); //postavimo pokazivac na pocetak fajla
			this->openFiles->add(openFile); //dodamo fajl u listu otvorenih fajlova
			return openFile;
			break;

		case 'a':
			openFile = new File();
			openFile->setName(fname);
			openFile->getKernelFile()->setMode(mode);
			openFile->seek(dirEntry->getSize(fname));
			this->openFiles->add(openFile);
			break;

		case 'w':

			break;
		}
	}

	//fajl postoji na disku i otvoren je
	if (exists && openFile) {
		switch (openFile->getKernelFile()->getMode())
		{
		case 'r':
			if (mode == 'r') {
				//
			}
			if (mode == 'w' || mode == 'a') {
				//blokiranje
			}
			break;

		case 'w':
			//blokiranje
			break;

		case 'a':
			//blokiranje
			break;
		}
	}
}

File * KernelFS::createFile()
{
	return nullptr;
}

char KernelFS::deleteFile(char * fname)
{
	return 0;
}
