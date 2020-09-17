#pragma once

#include "file.h"

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
	void remove(File *f);

	void deleteList();

	bool empty() const;

	File* isOpen(char *fname) const;

	FileElem *head, *tail;
};

