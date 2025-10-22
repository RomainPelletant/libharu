/* SPDX-License-Identifier: Apache-2.0 */
#include <string.h>
#include <hpdf.h>
#include <zephyr/ztest.h>

static HPDF_Doc pdf;
static HPDF_STATUS last_error;

static void error_handler(HPDF_STATUS error, HPDF_STATUS detail, void *user_data)
{
	HPDF_STATUS *saved_error = user_data;

	ARG_UNUSED(detail);
	*saved_error = error;
}

static void before(void *fixture)
{
	ARG_UNUSED(fixture);
	last_error = HPDF_OK;
	pdf = HPDF_New(error_handler, &last_error);
	zassert_not_null(pdf);
}

static void after(void *fixture)
{
	ARG_UNUSED(fixture);
	HPDF_Free(pdf);
	pdf = NULL;
}

ZTEST(libharu, test_document_lifecycle)
{
	zassert_true(HPDF_HasDoc(pdf));
	HPDF_FreeDoc(pdf);
	zassert_false(HPDF_HasDoc(pdf));
	zassert_equal(HPDF_NewDoc(pdf), HPDF_OK);
	zassert_true(HPDF_HasDoc(pdf));
	zassert_not_null(HPDF_AddPage(pdf));
}

ZTEST(libharu, test_metadata_and_pages)
{
	HPDF_Page first = HPDF_AddPage(pdf);
	HPDF_Page second = HPDF_AddPage(pdf);

	zassert_not_null(first);
	zassert_not_null(second);
	zassert_not_equal(first, second);
	zassert_equal(HPDF_Page_SetWidth(first, 300), HPDF_OK);
	zassert_equal(HPDF_Page_SetHeight(first, 400), HPDF_OK);
	zassert_equal(HPDF_Page_GetWidth(first), 300);
	zassert_equal(HPDF_Page_GetHeight(first), 400);
	zassert_equal(HPDF_SetInfoAttr(pdf, HPDF_INFO_TITLE, "Zephyr report"), HPDF_OK);
	zassert_str_equal(HPDF_GetInfoAttr(pdf, HPDF_INFO_TITLE), "Zephyr report");
}

#ifdef CONFIG_LIBHARU_FONT_HELVETICA
ZTEST(libharu, test_text_and_stream)
{
	HPDF_Page page = HPDF_AddPage(pdf);
	HPDF_Font font = HPDF_GetFont(pdf, "Helvetica", NULL);
	HPDF_BYTE buffer[37];
	HPDF_BYTE tail[6] = {0};
	HPDF_UINT32 total = 0;
	HPDF_UINT32 expected;
	HPDF_UINT32 size;

	zassert_not_null(page);
	zassert_not_null(font);
	zassert_equal(HPDF_Page_BeginText(page), HPDF_OK);
	zassert_equal(HPDF_Page_SetFontAndSize(page, font, 12), HPDF_OK);
	zassert_equal(HPDF_Page_TextOut(page, 50, 100, "Zephyr PDF"), HPDF_OK);
	zassert_equal(HPDF_Page_EndText(page), HPDF_OK);
	zassert_equal(HPDF_SaveToStream(pdf), HPDF_OK);
	expected = HPDF_GetStreamSize(pdf);
	zassert_true(expected > sizeof(buffer));
	zassert_equal(HPDF_ResetStream(pdf), HPDF_OK);

	while (total < expected) {
		HPDF_STATUS status;

		size = sizeof(buffer);
		status = HPDF_ReadFromStream(pdf, buffer, &size);
		zassert_true(status == HPDF_OK || status == HPDF_STREAM_EOF);
		zassert_true(size > 0 && size <= sizeof(buffer));
		if (total == 0) {
			zassert_mem_equal(buffer, "%PDF-", 5);
		}
		for (HPDF_UINT32 i = 0; i < size; i++) {
			memmove(tail, tail + 1, sizeof(tail) - 1);
			tail[sizeof(tail) - 1] = buffer[i];
		}
		total += size;
	}

	zassert_equal(total, expected);
	zassert_mem_equal(tail, "%%EOF\n", sizeof(tail));
	size = sizeof(buffer);
	zassert_equal(HPDF_ReadFromStream(pdf, buffer, &size), HPDF_STREAM_EOF);
	zassert_equal(size, 0);
	zassert_equal(HPDF_ResetStream(pdf), HPDF_OK);
	size = 5;
	zassert_equal(HPDF_ReadFromStream(pdf, buffer, &size), HPDF_OK);
	zassert_mem_equal(buffer, "%PDF-", 5);
}

#endif

ZTEST(libharu, test_error_callback_and_recovery)
{
	HPDF_Page page = HPDF_AddPage(pdf);

	zassert_not_null(page);
	zassert_equal(HPDF_Page_SetWidth(page, 0), HPDF_PAGE_INVALID_SIZE);
	zassert_equal(last_error, HPDF_PAGE_INVALID_SIZE);
	zassert_equal(HPDF_GetError(pdf), HPDF_PAGE_INVALID_SIZE);
	HPDF_ResetError(pdf);
	zassert_equal(HPDF_GetError(pdf), HPDF_OK);
	zassert_equal(HPDF_Page_SetWidth(page, 300), HPDF_OK);
}

ZTEST(libharu, test_raw_image)
{
	static const HPDF_BYTE pixels[] = {255, 0, 0, 0, 255, 0};
	HPDF_Page page = HPDF_AddPage(pdf);
	HPDF_Image image = HPDF_LoadRawImageFromMem(pdf, pixels, 2, 1,
						  HPDF_CS_DEVICE_RGB, 8);

	zassert_not_null(page);
	zassert_not_null(image);
	zassert_equal(HPDF_Image_GetWidth(image), 2);
	zassert_equal(HPDF_Image_GetHeight(image), 1);
	zassert_equal(HPDF_Page_DrawImage(page, image, 20, 20, 100, 50), HPDF_OK);
	zassert_equal(HPDF_SaveToStream(pdf), HPDF_OK);
	zassert_true(HPDF_GetStreamSize(pdf) > 0);
}

ZTEST_SUITE(libharu, NULL, NULL, before, after, NULL);
