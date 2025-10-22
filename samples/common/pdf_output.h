/* SPDX-License-Identifier: Apache-2.0 */
#ifndef SAMPLES_PDF_OUTPUT_H_
#define SAMPLES_PDF_OUTPUT_H_

#include <hpdf.h>

/* Sample-only transport: reconstruct the PDF with samples/capture_pdf.py. */
int pdf_output(HPDF_Doc pdf, const char *name);

#endif /* SAMPLES_PDF_OUTPUT_H_ */
