#pragma once

#include "file.h"
#include "dataRep.h"

struct FileElem {
	File *file;
	FileElem *next;

	FileElem(File *f) : file(f), next(nullptr) {}
};

class FileList
{
public:
	FileList();
	~FileList();

	void add(File *f);
	void remove(KernelFile *f);

	void deleteList();

	bool empty() const;

	File* isOpen(char *fname) const;

	FileElem *head, *tail;

	Sem mutex;
};

