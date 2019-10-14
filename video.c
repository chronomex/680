/* LCD controller driver for 680 project.
 *
 * Copyright 2010, Astrid Smith
 * GPL
 */

#include <graph.h>
#include <wingraph.h>
#include <alloc.h>


#define VIDEO_ROWMODE 0x01
#define VIDEO_COLMODE 0x00
char video_row;

#define VIDEO_AUTOINC 0x02
#define VIDEO_AUTODEC 0x00
char video_increment;

#define VIDEO_ENABLED 0x00
#define VIDEO_DISABLED 0x20
char video_enabled;

#define VIDEO_6BIT 0x00
#define VIDEO_8BIT 0x40
char video_6bit;

char video_busy;        // Always 0

char video_cur_row;
char video_cur_col;

WINDOW *screen_window;

void video_write(char);
char video_read(void);
void *video_compute_address(void);
int video_compute_shift(void);

void display_setup(void)
{
	screen_window = HeapAllocPtr(sizeof(WINDOW));
	WinOpen(screen_window, MakeWinRect(10, 10, 106, 74), WF_SAVE_SCR | WF_TTY | WF_ROUNDEDBORDER | WF_TITLE, "TI-83+ Emulator");
	WinActivate(screen_window);
	return;
}

void display_teardown(void)
{
	WinClose(screen_window);
	HeapFreePtr(screen_window);
}

void video_write(char data)
{
	int shift = video_compute_shift();
	short int data_mask = ~(0xff00 >> shift);
	short int data_dat = data >> (shift-8);
	void *addr = video_compute_address();
	*((short int *)addr) &= data_mask;
	*((short int *)addr) |= data_dat;
	return;
}

char video_read(void)
{
	int shift = video_compute_shift();
	void *addr = video_compute_address();
	short int data = *((short int *)addr);
	data <<= (shift-8);
	data &= 0xff;
	return data;
}

void *video_compute_address()
{
	void *addr;
	int off;

	addr = LCD_MEM;
	addr += (video_cur_row) * (240 / 8);

	if (video_6bit == VIDEO_6BIT)
		off = video_cur_col * 6 / 8;
	else
		off = video_cur_col;
	return addr + off;
}

int video_compute_shift()
{
	if (video_6bit == VIDEO_8BIT)
		return 0;

	return (video_cur_col * 6) % 8;
}
