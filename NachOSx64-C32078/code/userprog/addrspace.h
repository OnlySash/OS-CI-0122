// addrspace.h 
//	Data structures to keep track of executing user programs 
//	(address spaces).
//
//	For now, we don't keep any information about address spaces.
//	The user level CPU state is saved and restored in the thread
//	executing the user program (see thread.h).
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.
//
// Copyright (c) -2026 Universidad de Costa Rica

#ifndef ADDRSPACE_H
#define ADDRSPACE_H

#include "copyright.h"
#include "filesys.h"

#define UserStackSize		1024 	// increase this as necessary!

#define MAX_FILES 32

struct TableFiles {
    OpenFile *file[MAX_FILES];   // NULL si está libre
    bool used[MAX_FILES];          // 0, 1 y 2 reservados para la consola

    TableFiles() {
        for (int i = 0; i < MAX_FILES; i++) {
            file[i] = NULL;
            used[i] = (i < 3);
        }
    }
};

class AddrSpace {
  public:
    AddrSpace(OpenFile *executable);	// Create an address space,
					// initializing it with the program
					// stored in the file "executable"
    ~AddrSpace();			// De-allocate an address space

    void InitRegisters();		// Initialize user-level CPU registers,
					// before jumping to user code

    void SaveState();			// Save/restore address space-specific
    void RestoreState();		// info on a context switch 
    TableFiles *table;

  private:
    TranslationEntry *pageTable;	// Assume linear page table translation
					// for now!
    unsigned int numPages;		// Number of pages in the virtual 
					// address space
    void charge_segment(OpenFile *executable, int virtual_addr,int size, int in_file_addr);
};

#endif // ADDRSPACE_H
