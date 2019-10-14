/* Debugging routines for 680 project.
 *
 * Includes debug output.
 *
 * Copyright 2010, Astrid Smith
 * GPL
 */

//#include <stdio.h>

short putchar(short c)
{
	return c;
}


void char_draw(char c)
{
	putchar((short)c);
}

