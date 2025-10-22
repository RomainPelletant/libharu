/* SPDX-License-Identifier: Apache-2.0 */
#include <errno.h>
#include <zephyr/sys/printk.h>

#include "pdf_output.h"

int main(void)
{
	HPDF_Doc pdf = HPDF_New(NULL, NULL);
	HPDF_Page page;
	HPDF_Font font;
	int ret = -EIO;

	if (pdf == NULL) {
		printk("Cannot allocate PDF document\n");
		return -ENOMEM;
	}

	page = HPDF_AddPage(pdf);
	font = HPDF_GetFont(pdf, "Helvetica", NULL);
	if (page == NULL || font == NULL) {
		goto out;
	}

	if (HPDF_SetInfoAttr(pdf, HPDF_INFO_TITLE, "Hello from Zephyr") != HPDF_OK ||
	    HPDF_Page_SetSize(page, HPDF_PAGE_SIZE_A4, HPDF_PAGE_PORTRAIT) != HPDF_OK ||
	    HPDF_Page_BeginText(page) != HPDF_OK ||
	    HPDF_Page_SetFontAndSize(page, font, 24) != HPDF_OK ||
	    HPDF_Page_TextOut(page, 50, 780, "Hello from Zephyr!") != HPDF_OK ||
	    HPDF_Page_EndText(page) != HPDF_OK) {
		goto out;
	}

	ret = pdf_output(pdf, "hello.pdf");
out:
	if (ret != 0) {
		printk("PDF generation failed: 0x%lx\n", (unsigned long)HPDF_GetError(pdf));
	}
	HPDF_Free(pdf);
	return ret;
}
