/*
 * << Haru Free PDF Library >> -- hpdf_pdfa.h
 *
 * URL: http://libharu.org
 *
 * Copyright (c) 1999-2006 Takeshi Kanno <takeshi_kanno@est.hi-ho.ne.jp>
 * Copyright (c) 2007-2009 Antony Dovgal <tony@daylessday.org>
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.
 * It is provided "as is" without express or implied warranty.
 *
 */

#ifndef _HPDF_PDFA_H
#define _HPDF_PDFA_H

#include "hpdf.h"
#include "hpdf_doc.h"
#include "hpdf_objects.h"



#ifdef __cplusplus
extern "C" {
#endif

#if !defined(__ZEPHYR__) || defined(CONFIG_LIBHARU_PDFA)

HPDF_STATUS
HPDF_PDFA_AppendOutputIntents(HPDF_Doc pdf, const char *iccname, HPDF_Dict iccdict);

HPDF_EXPORT(HPDF_STATUS)
HPDF_PDFA_SetPDFAConformance(HPDF_Doc pdf,
                             HPDF_PDFAType pdfatype);

HPDF_STATUS
HPDF_PDFA_AddXmpMetadata(HPDF_Doc pdf);

HPDF_STATUS
HPDF_PDFA_AddXmpExtension(HPDF_Doc    pdf,
                          const char *xmp_description);

void
HPDF_PDFA_ClearXmpExtensions(HPDF_Doc pdf);

HPDF_STATUS
HPDF_PDFA_GenerateID(HPDF_Doc);

#else /* Zephyr: hpdf_pdfa.c is not built, PDF/A APIs fail */

static inline HPDF_STATUS
HPDF_PDFA_AppendOutputIntents(HPDF_Doc pdf, const char *iccname, HPDF_Dict iccdict)
{
    (void)iccname; (void)iccdict;
    return HPDF_RaiseError(&pdf->error, HPDF_UNSUPPORTED_FUNC, 0);
}

static inline HPDF_STATUS
HPDF_PDFA_SetPDFAConformance(HPDF_Doc pdf,
                             HPDF_PDFAType pdfatype)
{
    (void)pdfatype;
    return HPDF_RaiseError(&pdf->error, HPDF_UNSUPPORTED_FUNC, 0);
}

static inline HPDF_STATUS
HPDF_PDFA_AddXmpMetadata(HPDF_Doc pdf)
{
    return HPDF_RaiseError(&pdf->error, HPDF_UNSUPPORTED_FUNC, 0);
}

static inline HPDF_STATUS
HPDF_PDFA_AddXmpExtension(HPDF_Doc    pdf,
                          const char *xmp_description)
{
    (void)xmp_description;
    return HPDF_RaiseError(&pdf->error, HPDF_UNSUPPORTED_FUNC, 0);
}

static inline void
HPDF_PDFA_ClearXmpExtensions(HPDF_Doc pdf)
{
    (void)pdf;
}

#endif
#ifdef __cplusplus
}
#endif

#endif
