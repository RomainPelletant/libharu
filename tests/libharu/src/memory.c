/* SPDX-License-Identifier: Apache-2.0 */
#include <stdlib.h>
#include <hpdf.h>
#include <sys_malloc.h>
#include <zephyr/ztest.h>

/* Upper bound on allocations for the small document below. */
#define MAX_ALLOCATIONS 5000

static HPDF_STATUS last_error;

static void error_handler(HPDF_STATUS error, HPDF_STATUS detail, void *user_data)
{
	ARG_UNUSED(detail);
	ARG_UNUSED(user_data);
	last_error = error;
}

/* Exercise text, graphics, an image and serialization; report the first failure. */
static HPDF_STATUS build_document(HPDF_Doc pdf)
{
	static const HPDF_BYTE pixels[] = {255, 0, 0, 0, 255, 0};
	HPDF_Page page = HPDF_AddPage(pdf);
	HPDF_Image image;

	if (page == NULL) {
		return HPDF_GetError(pdf);
	}
#ifdef CONFIG_LIBHARU_FONT_HELVETICA
	HPDF_Font font = HPDF_GetFont(pdf, "Helvetica", NULL);

	if (font == NULL || HPDF_Page_BeginText(page) != HPDF_OK ||
	    HPDF_Page_SetFontAndSize(page, font, 12) != HPDF_OK ||
	    HPDF_Page_TextOut(page, 50, 100, "Memory test") != HPDF_OK ||
	    HPDF_Page_EndText(page) != HPDF_OK) {
		return HPDF_GetError(pdf);
	}
#endif
	if (HPDF_Page_Rectangle(page, 20, 20, 200, 100) != HPDF_OK ||
	    HPDF_Page_Stroke(page) != HPDF_OK) {
		return HPDF_GetError(pdf);
	}
	image = HPDF_LoadRawImageFromMem(pdf, pixels, 2, 1, HPDF_CS_DEVICE_RGB, 8);
	if (image == NULL || HPDF_Page_DrawImage(page, image, 20, 140, 100, 50) != HPDF_OK) {
		return HPDF_GetError(pdf);
	}
	if (HPDF_SetInfoAttr(pdf, HPDF_INFO_TITLE, "Memory test") != HPDF_OK) {
		return HPDF_GetError(pdf);
	}
	return HPDF_SaveToStream(pdf);
}

static size_t heap_allocated(void)
{
	struct sys_memory_stats stats;

	zassert_ok(malloc_runtime_stats_get(&stats));
	return stats.allocated_bytes;
}

ZTEST(memory, test_no_leak_after_free)
{
	size_t baseline = heap_allocated();

	/* Repeat to catch growth that a single cycle could hide. */
	for (int i = 0; i < 3; i++) {
		HPDF_Doc pdf = HPDF_New(error_handler, NULL);

		zassert_not_null(pdf);
		zassert_equal(build_document(pdf), HPDF_OK);
		HPDF_Free(pdf);
		zassert_equal(heap_allocated(), baseline, "cycle %d leaked %zu bytes", i,
			      heap_allocated() - baseline);
	}
}

ZTEST(memory, test_unbalanced_gsave_no_leak)
{
	size_t baseline = heap_allocated();
	HPDF_Doc pdf = HPDF_New(error_handler, NULL);
	HPDF_Page page;

	zassert_not_null(pdf);
	page = HPDF_AddPage(pdf);
	zassert_not_null(page);
	/* Saved graphics states must be released even without GRestore. */
	zassert_equal(HPDF_Page_GSave(page), HPDF_OK);
	zassert_equal(HPDF_Page_GSave(page), HPDF_OK);
	HPDF_Free(pdf);
	zassert_equal(heap_allocated(), baseline);
}

/* Allocator that fails once its budget is spent and counts live blocks. */
static int alloc_budget;
static int live_blocks;

static void *budget_alloc(HPDF_UINT size)
{
	void *ptr;

	if (alloc_budget == 0) {
		return NULL;
	}
	ptr = malloc(size);
	if (ptr != NULL) {
		alloc_budget--;
		live_blocks++;
	}
	return ptr;
}

static void budget_free(void *ptr)
{
	if (ptr != NULL) {
		live_blocks--;
		free(ptr);
	}
}

ZTEST(memory, test_allocation_failures)
{
	size_t baseline = heap_allocated();
	int budget;

	/* Fail at every allocation in turn until the whole document succeeds. */
	for (budget = 0; budget < MAX_ALLOCATIONS; budget++) {
		HPDF_STATUS status = HPDF_FAILED_TO_ALLOC_MEM;
		HPDF_Doc pdf;

		alloc_budget = budget;
		live_blocks = 0;
		last_error = HPDF_OK;
		/* No memory pool, so each object goes through budget_alloc(). */
		pdf = HPDF_NewEx(error_handler, budget_alloc, budget_free, 0, NULL);
		if (pdf != NULL) {
			status = build_document(pdf);
			HPDF_Free(pdf);
		}
		zassert_equal(live_blocks, 0, "failure at allocation %d leaked %d blocks",
			      budget, live_blocks);
		if (status == HPDF_OK) {
			break;
		}
		if (pdf != NULL) {
			zassert_not_equal(last_error, HPDF_OK,
					  "failure at allocation %d was not reported", budget);
		}
	}

	zassert_true(budget > 0, "document built without allocating");
	zassert_true(budget < MAX_ALLOCATIONS, "document never completed");
	zassert_equal(heap_allocated(), baseline);
	TC_PRINT("document needs %d allocations\n", budget);
}

ZTEST_SUITE(memory, NULL, NULL, NULL, NULL, NULL);
