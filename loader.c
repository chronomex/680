/* Loading routines for 680 project.
 *
 * Includes splash screen, choose-an-image, etc.
 *
 * Copyright 2010, Astrid Smith
 * GPL
 */
#include <tigcclib.h>
#include "asm_vars.h"

#include "global.h"
#include "image.h"

HANDLE page_handles[256];

char infloop[16] = { 0xC3, 0x40,	// JP 4000h
		     0, 0, 0, 0 };
char writestr[16] = { 0x3E, 0x41,	// LD A,'A'
		      0xD3, 0x00,	// OUT 00h,A
		      0xC3, 0x40, 0x00	// JP 4000h
};

#include "testbenches/zexdoc.testbench.h"

void init_load(void);
void unload(void);
void *deref_page(int);
void close_pages(void);

void init_load(void)
{
	int i;

	pages = malloc(256 * sizeof(void*));

	/* Page layout:
	 * 0x40  RAM
	 * 0x41  RAM
	 *
	 * 0x00  ROM
	 * ... all the way to ...
	 * 0x1f  ROM
	 */

	for (i = 0 ; i <= 255; i++)
		page_handles[i] = 0;

	i = 0;

	// RAM pages
	pages[0x40] = deref_page(0x40);
	pages[0x41] = deref_page(0x41);

	if (pages[0x40] == NULL)
		pages[0x40] = malloc(PAGE_SIZE * sizeof(char));
	if (pages[0x41] == NULL)
		pages[0x41] = malloc(PAGE_SIZE * sizeof(char));


	// ROM pages
	for (i = 0; i <= 0x1f; i++) {
		pages[i] = deref_page(i);
		if (pages[i] == NULL)
			pages[i] = pages[0x40];
	}

	mem_page_0 = pages[0];
	mem_page_loc_0 = 0;
//	mem_page_1 = pages[0x1f];
	mem_page_1 = testbench;
	mem_page_loc_1 = 0x1f;
	mem_page_2 = pages[0];
	mem_page_loc_2 = 0;
	mem_page_3 = pages[0x40];
	mem_page_loc_3 = 0x40;

	return;

}

/* Loads an image file into the emulator, restoring state.
 */
void load_descriptor(char *folder, char *descriptor)
{
	
}

/* Load a page into the emulator's data structures.  Only stores a
 * pointer to it; does not copy due to space constraints.
 *
 * ROM pages can (and should) be archived, but RAM pages must be
 * unarchived.
 */
void load_page(int pageno, WORD *pptr)
{
	/* check the version */
	if (*pptr != IMG_MAGIC)
		ER_throw(ER_DATATYPE);

	pptr++;

	/* check the size */
	if (*pptr != 0x4000)
		ER_throw(ER_INVALID_BLOCK_STRUCTURE);

	pptr++;

	if (*pptr != (WORD)pageno)
		ER_throw(ER_INVALID_LABEL);

	pptr++;

	if (*pptr != (WORD)c_TI83P)
		ER_throw(ILLEGAL_TAG_ERROR);

	pptr++;

	pages[pageno] = pptr;
}

void unload(void)
{
	return;
}

/* Turns a page number into a pointer to a page.  Returns NULL if not
 * found, throws an error in other cases.
 */
void *deref_page(int number)
{
/* Bits of code here stolen from MulTI */

	char page_name[8];
	HSYM hsym;
	HANDLE fhandle;
	char *fdata;
	int fsize;

	sprintf(page_name, "pg_%02x", number);
	hsym = SymFind(SYMSTR(page_name));

	if(hsym.folder == 0)
		return NULL;

	fhandle = DerefSym(hsym)->handle;

	fdata = HLock(fhandle);
	if(fdata == NULL)
		throw_error("Couldn't lock page");

	page_handles[number] = fhandle;

	/* read size */
	fsize = *(WORD *)fdata;
	fdata += 2;

	/* check type */
	if((fdata[fsize - 1] != OTH_TAG) || strcmp(&fdata[fsize - 6], "83p")) {
		close_pages();
		throw_error("Not a 680 file");
	}

	return fdata;
}

void close_pages(void)
{
	int i;

	for (i = 0; i < 256; i++)
		if (page_handles[i] != 0)
			HeapUnlock(page_handles[i]);
}


