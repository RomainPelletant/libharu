/* SPDX-License-Identifier: Apache-2.0 */
#include <errno.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>

#include "pdf_output.h"

/* A 2 x 2 RGB image; no filesystem, JPEG or PNG decoder is needed. */
static const HPDF_BYTE pixels[] = {
	255, 0, 0, 0, 255, 0,
	0, 0, 255, 255, 255, 0,
};
static const HPDF_REAL values[] = {80, 140, 110, 200, 160};

int main(void)
{
	HPDF_Doc pdf = HPDF_New(NULL, NULL);
	HPDF_Page page;
	HPDF_Image image;
	int ret = -EIO;

	if (pdf == NULL) {
		return -ENOMEM;
	}

	page = HPDF_AddPage(pdf);
	image = HPDF_LoadRawImageFromMem(pdf, pixels, 2, 2, HPDF_CS_DEVICE_RGB, 8);
	if (page == NULL || image == NULL ||
	    HPDF_Page_SetSize(page, HPDF_PAGE_SIZE_A4, HPDF_PAGE_PORTRAIT) != HPDF_OK ||
	    HPDF_Page_SetRGBFill(page, 0.1f, 0.4f, 0.8f) != HPDF_OK) {
		goto out;
	}

	for (size_t i = 0; i < ARRAY_SIZE(values); i++) {
		if (HPDF_Page_Rectangle(page, 50 + i * 90, 400, 60, values[i]) != HPDF_OK ||
		    HPDF_Page_Fill(page) != HPDF_OK) {
			goto out;
		}
	}

	if (HPDF_Page_SetLineWidth(page, 2) != HPDF_OK ||
	    HPDF_Page_MoveTo(page, 40, 400) != HPDF_OK ||
	    HPDF_Page_LineTo(page, 530, 400) != HPDF_OK ||
	    HPDF_Page_Stroke(page) != HPDF_OK ||
	    HPDF_Page_DrawImage(page, image, 50, 150, 160, 160) != HPDF_OK) {
		goto out;
	}

	ret = pdf_output(pdf, "graphics.pdf");
out:
	if (ret != 0) {
		printk("PDF generation failed: 0x%lx\n", (unsigned long)HPDF_GetError(pdf));
	}
	HPDF_Free(pdf);
	return ret;
}
