/* Memory bank swapping driver for 680 project.
 *
 * Copyright 2010, Astrid Smith
 * GPL
 */

#include "asm_vars.h"

/* Address 0000 is always bound to ROM page 0. */

/* Process orders to swap bank A (port 06, 0x4000). */
void bankswap_a_write(char data)
{
	mem_page_1 = pages[(int)data];
	mem_page_loc_1 = data;
	return;
}

/* Process orders to swap bank B (port 07, 0x8000). */
void bankswap_b_write(char data)
{
	mem_page_2 = pages[(int)data];
	mem_page_loc_2 = data;
	return;
}
