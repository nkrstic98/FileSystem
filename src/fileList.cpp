#include "fileList.h"
#include "kernelfile.h"
#include "kernelfs.h"

FileList::FileList() : head(nullptr), tail(nullptr) {
	mutex = CreateSemaphore(0, 1, 1, NULL);
}

FileList::~FileList()
{
	deleteList();
	delete head;
	delete tail;
}

void FileList::add(File * f)
{
	wait(mutex);

	FileElem *elem = new FileElem(f);

	if (head == nullptr) head = tail = elem;
	else {
		tail->next = elem;
		tail = elem;
	}

	signal(mutex);
}

void FileList::remove(KernelFile * f)
{
	wait(mutex);

	FileElem *temp, *prev;

	if (head == nullptr) {
		signal(mutex);
		return;
	}

	if (head->file->getKernelFile() == f) {
		temp = head;
		head = head->next;
		delete temp;
	}
	else {
		temp = head;
		while (temp->file->getKernelFile() != f) {
			prev = temp;
			temp = temp->next;
			if (temp == nullptr) break;
		}
		if (temp != nullptr) {
			prev->next = temp->next;
			temp->next = nullptr;
			delete temp;
		}
	}

	signal(mutex);
}

void FileList::deleteList()
{
	wait(mutex);

	FileElem *temp = head, *old;

	while (temp != nullptr) {
		old = temp;
		temp = temp->next;
		delete old;
	}

	delete head;
	delete tail;

	signal(mutex);
}

bool FileList::empty() const
{
	wait(mutex);

	if (head != nullptr) {
		signal(mutex);
		return false;
	}
	else {
		signal(mutex);
		return true;
	}
}

File* FileList::isOpen(char * fname) const
{
	wait(mutex);

	FileElem* temp = head;

	while (temp != nullptr) {
		if (strcmp(FS::getKernelFS()->dirEntry->getName(temp->file->getKernelFile()->getMyEntry()), fname) == 0) {
			signal(mutex);
			return temp->file;
		}
		temp = temp->next;
	}

	signal(mutex);

	return nullptr;
}
