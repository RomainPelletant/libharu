/* SPDX-License-Identifier: Apache-2.0 */
#include <errno.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>

#include "pdf_output.h"

int pdf_output(HPDF_Doc pdf, const char *name)
{
	HPDF_BYTE buffer[32];
	HPDF_UINT32 remaining;
	HPDF_STATUS status;

	status = HPDF_SaveToStream(pdf);
	if (status != HPDF_OK) {
		return -EIO;
	}

	remaining = HPDF_GetStreamSize(pdf);
	if (remaining == 0 || HPDF_ResetStream(pdf) != HPDF_OK) {
		return -EIO;
	}

	printk("PDF-BEGIN %s %lu\n", name, (unsigned long)remaining);
	while (remaining > 0) {
		HPDF_UINT32 size = MIN(remaining, sizeof(buffer));

		status = HPDF_ReadFromStream(pdf, buffer, &size);
		if ((status != HPDF_OK && status != HPDF_STREAM_EOF) || size == 0) {
			return -EIO;
		}

		printk("PDF-DATA ");
		for (HPDF_UINT32 i = 0; i < size; i++) {
			printk("%02x", (unsigned int)buffer[i]);
		}
		printk("\n");
		remaining -= size;
	}
	printk("PDF-END\n");

	return 0;
}
