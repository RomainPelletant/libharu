// SPDX-License-Identifier: Apache-2.0
#include <hpdf.h>
#include <zephyr/sys/util.h>
#include <zephyr/ztest.h>

static HPDF_Doc pdf;

static void before(void *fixture)
{
	ARG_UNUSED(fixture);
	pdf = HPDF_New(NULL, NULL);
	zassert_not_null(pdf);
}

static void after(void *fixture)
{
	ARG_UNUSED(fixture);
	HPDF_Free(pdf);
	pdf = NULL;
}

ZTEST(components, test_builtin_font_selection)
{
	static const struct {
		const char *name;
		bool enabled;
	} fonts[] = {
		{"Helvetica", IS_ENABLED(CONFIG_LIBHARU_FONT_HELVETICA)},
		{"Helvetica-BoldOblique", IS_ENABLED(CONFIG_LIBHARU_FONT_HELVETICA)},
		{"Courier", IS_ENABLED(CONFIG_LIBHARU_FONT_COURIER)},
		{"Courier-BoldOblique", IS_ENABLED(CONFIG_LIBHARU_FONT_COURIER)},
		{"Times-Roman", IS_ENABLED(CONFIG_LIBHARU_FONT_TIMES)},
		{"Times-BoldItalic", IS_ENABLED(CONFIG_LIBHARU_FONT_TIMES)},
		{"Symbol", IS_ENABLED(CONFIG_LIBHARU_FONT_SYMBOL)},
		{"ZapfDingbats", IS_ENABLED(CONFIG_LIBHARU_FONT_ZAPF_DINGBATS)},
	};

	for (size_t i = 0; i < ARRAY_SIZE(fonts); i++) {
		HPDF_Font font = HPDF_GetFont(pdf, fonts[i].name, NULL);

		if (fonts[i].enabled) {
			zassert_not_null(font, "%s must be available", fonts[i].name);
		} else {
			zassert_is_null(font, "%s must be excluded", fonts[i].name);
			zassert_equal(HPDF_GetError(pdf), HPDF_INVALID_FONT_NAME);
		}
		HPDF_ResetError(pdf);
	}
}

#ifdef CONFIG_LIBHARU_TRUETYPE
ZTEST(components, test_invalid_truetype)
{
	static const HPDF_BYTE invalid_font[16] = {0};

	zassert_is_null(HPDF_LoadTTFontFromMemory(pdf, invalid_font,
						sizeof(invalid_font), HPDF_TRUE));
	zassert_not_equal(HPDF_GetError(pdf), HPDF_OK);
}
#endif

#ifdef CONFIG_LIBHARU_ENCODING_UTF
ZTEST(components, test_utf8_registration)
{
	zassert_equal(HPDF_UseUTFEncodings(pdf), HPDF_OK);
	zassert_not_null(HPDF_GetEncoder(pdf, "UTF-8"));
}
#endif

#ifdef CONFIG_LIBHARU_ENCRYPTION
ZTEST(components, test_encrypted_document)
{
	zassert_not_null(HPDF_AddPage(pdf));
	zassert_equal(HPDF_SetPassword(pdf, "owner", "reader"), HPDF_OK);
	zassert_equal(HPDF_SetPermission(pdf, HPDF_ENABLE_READ), HPDF_OK);
	zassert_equal(HPDF_SetEncryptionMode(pdf, HPDF_ENCRYPT_R3, 16), HPDF_OK);
	zassert_equal(HPDF_SaveToStream(pdf), HPDF_OK);
	zassert_true(HPDF_GetStreamSize(pdf) > 0);
}
#endif

#ifdef CONFIG_LIBHARU_PDFA
ZTEST(components, test_pdfa_metadata)
{
	zassert_not_null(HPDF_AddPage(pdf));
	zassert_equal(HPDF_SetPDFAConformance(pdf, HPDF_PDFA_1B), HPDF_OK);
	zassert_equal(HPDF_SetInfoAttr(pdf, HPDF_INFO_TITLE, "PDF/A metadata"), HPDF_OK);
	zassert_equal(HPDF_SaveToStream(pdf), HPDF_OK);
	zassert_true(HPDF_GetStreamSize(pdf) > 0);
}
#endif

#ifdef CONFIG_LIBHARU_FONT_JP
ZTEST(components, test_jp_registration)
{
	zassert_equal(HPDF_UseJPFonts(pdf), HPDF_OK);
	zassert_equal(HPDF_UseJPEncodings(pdf), HPDF_OK);
	zassert_not_null(HPDF_GetFont(pdf, "MS-Mincho", "90ms-RKSJ-H"));
}
#endif

#ifdef CONFIG_LIBHARU_FONT_KR
ZTEST(components, test_kr_registration)
{
	zassert_equal(HPDF_UseKRFonts(pdf), HPDF_OK);
	zassert_equal(HPDF_UseKREncodings(pdf), HPDF_OK);
	zassert_not_null(HPDF_GetFont(pdf, "DotumChe", "KSC-EUC-H"));
}
#endif

#ifdef CONFIG_LIBHARU_FONT_CNS
ZTEST(components, test_cns_registration)
{
	zassert_equal(HPDF_UseCNSFonts(pdf), HPDF_OK);
	zassert_equal(HPDF_UseCNSEncodings(pdf), HPDF_OK);
	zassert_not_null(HPDF_GetFont(pdf, "SimSun", "GB-EUC-H"));
}
#endif

#ifdef CONFIG_LIBHARU_FONT_CNT
ZTEST(components, test_cnt_registration)
{
	zassert_equal(HPDF_UseCNTFonts(pdf), HPDF_OK);
	zassert_equal(HPDF_UseCNTEncodings(pdf), HPDF_OK);
	zassert_not_null(HPDF_GetFont(pdf, "MingLiU", "ETen-B5-H"));
}
#endif

ZTEST_SUITE(components, NULL, NULL, before, after, NULL);
