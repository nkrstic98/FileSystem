#include "fileList.h"
#include "kernelfile.h"

FileList::FileList() : head(nullptr), tail(nullptr) {}

FileList::~FileList()
{
	deleteList();
	delete head;
	delete tail;
}

void FileList::add(File * f)
{
	FileElem *elem = new FileElem(f);

	if (head == nullptr) head = tail = elem;
	else {
		tail->next = elem;
		tail = elem;
	}
}

void FileList::remove(File * f)
{
	FileElem *temp, *prev;

	if (head->file == f) {
		temp = head;
		head = head->next;
		delete temp;
	}
	else {
		temp = head;
		while (temp->file != f) {
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
}

void FileList::deleteList()
{
	FileElem *temp = head, *old;

	while (temp != nullptr) {
		old = temp;
		temp = temp->next;
		delete old;
	}

	delete head;
	delete tail;
}

bool FileList::empty() const
{
	if (head != nullptr) return false;
	else return true;
}

File* FileList::open(char * fname) const
{
	FileElem* temp = head;

	while (temp != nullptr) {
		if (strcmp(temp->file->getName(), fname) == 0) return temp->file;
		temp = temp->next;
	}

	return nullptr;
}
