/* SPDX-License-Identifier: Apache-2.0 */
#include <errno.h>
#include <stdio.h>
#include <zephyr/sys/printk.h>

#include "pdf_output.h"

int main(void)
{
	HPDF_Doc pdf = HPDF_New(NULL, NULL);
	HPDF_Font font;
	int ret = -EIO;

	if (pdf == NULL) {
		return -ENOMEM;
	}

	font = HPDF_GetFont(pdf, "Courier", NULL);
	if (font == NULL ||
	    HPDF_SetInfoAttr(pdf, HPDF_INFO_TITLE, "Sensor report") != HPDF_OK) {
		goto out;
	}

	for (int index = 0; index < 3; index++) {
		HPDF_Page page = HPDF_AddPage(pdf);
		char line[64];
		int length;

		if (page == NULL ||
		    HPDF_Page_SetSize(page, HPDF_PAGE_SIZE_A4, HPDF_PAGE_PORTRAIT) != HPDF_OK ||
		    HPDF_Page_BeginText(page) != HPDF_OK ||
		    HPDF_Page_SetFontAndSize(page, font, 12) != HPDF_OK) {
			goto out;
		}

		length = snprintf(line, sizeof(line), "Sensor report - page %d of 3", index + 1);
		if (length < 0 || (size_t)length >= sizeof(line) ||
		    HPDF_Page_TextOut(page, 50, 790, line) != HPDF_OK ||
		    HPDF_Page_TextOut(page, 50, 760, "Sample       Temperature (C)") != HPDF_OK) {
			goto out;
		}

		for (int row = 0; row < 20; row++) {
			int sample = index * 20 + row;

			length = snprintf(line, sizeof(line), "%6d       %d.%d", sample,
					  20 + sample % 5, sample % 10);
			if (length < 0 || (size_t)length >= sizeof(line) ||
			    HPDF_Page_TextOut(page, 50, 730 - row * 24, line) != HPDF_OK) {
				goto out;
			}
		}

		if (HPDF_Page_EndText(page) != HPDF_OK) {
			goto out;
		}
	}

	ret = pdf_output(pdf, "report.pdf");
out:
	if (ret != 0) {
		printk("PDF generation failed: 0x%lx\n", (unsigned long)HPDF_GetError(pdf));
	}
	HPDF_Free(pdf);
	return ret;
}
