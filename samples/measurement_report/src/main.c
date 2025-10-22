/* SPDX-License-Identifier: Apache-2.0 */
#include <errno.h>
#include <stdio.h>
#include <zephyr/sys/printk.h>
#include "pdf_output.h"

/* Deterministic demonstration data: temperature in tenths of a degree C. */
static const int temperature[] = {
	214, 216, 215, 218, 221, 224, 228, 231, 235, 239, 243, 247,
	251, 246, 241, 237, 234, 230, 227, 225, 222, 219, 217, 215,
};
static const int humidity[] = {
	48, 48, 49, 48, 47, 47, 46, 46, 45, 44, 44, 43,
	42, 43, 44, 45, 45, 46, 47, 47, 48, 49, 49, 48,
};
#define COUNT (sizeof(temperature) / sizeof(temperature[0]))
#define LIMIT 250
#define WIDTH 595.276f
#define HEIGHT 841.89f

typedef struct { HPDF_REAL r, g, b; } Color;
static const Color navy = {0.06f, 0.13f, 0.21f};
static const Color teal = {0.0f, 0.48f, 0.47f};
static const Color muted = {0.38f, 0.44f, 0.49f};
static const Color paper = {0.97f, 0.97f, 0.95f};
static const Color white = {1, 1, 1};
static const Color pale = {0.88f, 0.94f, 0.92f};
static const Color amber = {0.77f, 0.32f, 0.10f};
static const Color warm = {1.0f, 0.93f, 0.86f};
static HPDF_Font regular, bold;
static HPDF_STATUS error_code;

static void on_error(HPDF_STATUS error, HPDF_STATUS detail, void *user_data)
{
	(void)detail;
	(void)user_data;
	if (!error_code) {
		error_code = error;
	}
}

/* All drawing coordinates use a top-left origin, in PDF points. */
static void box(HPDF_Page p, float x, float y, float w, float h, Color c)
{
	HPDF_Page_SetRGBFill(p, c.r, c.g, c.b);
	HPDF_Page_Rectangle(p, x, HEIGHT - y - h, w, h);
	HPDF_Page_Fill(p);
}

static void text(HPDF_Page p, float x, float y, float size,
		HPDF_Font font, Color c, const char *s)
{
	HPDF_Page_SetRGBFill(p, c.r, c.g, c.b);
	HPDF_Page_BeginText(p);
	HPDF_Page_SetFontAndSize(p, font, size);
	HPDF_Page_TextOut(p, x, HEIGHT - y, s);
	HPDF_Page_EndText(p);
}

static void line(HPDF_Page p, float x1, float y1, float x2, float y2,
		 float width, Color c)
{
	HPDF_Page_SetRGBStroke(p, c.r, c.g, c.b);
	HPDF_Page_SetLineWidth(p, width);
	HPDF_Page_MoveTo(p, x1, HEIGHT - y1);
	HPDF_Page_LineTo(p, x2, HEIGHT - y2);
	HPDF_Page_Stroke(p);
}

static void decimal(char *out, size_t size, int value)
{
	snprintf(out, size, "%d.%d", value / 10, value % 10);
}

static HPDF_Page page(HPDF_Doc pdf, int number, const char *section)
{
	HPDF_Page p = HPDF_AddPage(pdf);
	char label[24];

	if (!p) {
		return NULL;
	}
	HPDF_Page_SetSize(p, HPDF_PAGE_SIZE_A4, HPDF_PAGE_PORTRAIT);
	box(p, 0, 0, WIDTH, HEIGHT, paper);
	box(p, 36, 34, 7, 14, teal);
	text(p, 51, 45, 11, bold, navy, "FIELD NOTES / LAB 04");
	text(p, 365, 45, 9, regular, muted, "ENVIRONMENTAL MONITORING");
	line(p, 36, 782, 559, 782, 0.5f, muted);
	text(p, 36, 801, 8, regular, muted, "MR-024  /  SIMULATED DATA  /  14 OCT 2026");
	snprintf(label, sizeof(label), "%02d / 02", number);
	text(p, 519, 801, 9, bold, navy, label);
	text(p, 36, 73, 9, bold, teal, section);
	return p;
}

static void metric(HPDF_Page p, float x, const char *label, const char *value,
		   const char *detail)
{
	box(p, x, 220, 167, 98, white);
	box(p, x, 220, 167, 3, teal);
	text(p, x + 14, 246, 8, bold, muted, label);
	text(p, x + 14, 278, 25, bold, navy, value);
	text(p, x + 14, 300, 9, regular, muted, detail);
}

static float chart_y(int value)
{
	return 560.0f - (value - 200) * 2.6f;
}

static void overview(HPDF_Page p, int minimum, int maximum, int average, int alerts)
{
	char value[32], detail[40];

	text(p, 36, 118, 34, bold, navy, "Measurement report.");
	text(p, 36, 145, 12, regular, muted, "Thermal profile and humidity / controlled-room observation");
	box(p, 36, 166, 523, 31, navy);
	text(p, 49, 186, 9, regular, white,
	     "NODE TH-04    |    08:00 - 19:30    |    30-MIN INTERVAL    |    24 READINGS");
	decimal(value, sizeof(value), average);
	metric(p, 36, "MEAN TEMPERATURE / C", value, "Arithmetic mean of 24 readings");
	snprintf(value, sizeof(value), "%d.%d - %d.%d", minimum / 10, minimum % 10,
		 maximum / 10, maximum % 10);
	metric(p, 214, "OBSERVED RANGE / C", value, "Minimum to maximum");
	snprintf(value, sizeof(value), "%02d / 24", alerts);
	metric(p, 392, "ABOVE 25.0 C", value, "Threshold: strictly greater than");
	text(p, 36, 351, 17, bold, navy, "01  /  Temperature trend");
	text(p, 36, 372, 10, regular, muted, "Temperature (C) against local time. All points are simulated.");
	box(p, 36, 388, 523, 230, white);
	box(p, 76, chart_y(260), 457, chart_y(250) - chart_y(260), warm);
	for (int tick = 200; tick <= 260; tick += 20) {
		char label[8];
		float y = chart_y(tick);

		line(p, 76, y, 533, y, 0.5f, pale);
		snprintf(label, sizeof(label), "%d", tick / 10);
		text(p, 49, y + 3, 9, regular, muted, label);
	}
	line(p, 76, chart_y(LIMIT), 533, chart_y(LIMIT), 0.8f, amber);
	text(p, 404, chart_y(LIMIT) - 7, 8, bold, amber, "UPPER THRESHOLD 25.0 C");
	for (size_t i = 0; i < COUNT; i++) {
		float x = 76 + i * (457.0f / (COUNT - 1));
		float y = chart_y(temperature[i]);

		if (i) {
			line(p, x - 457.0f / (COUNT - 1), chart_y(temperature[i - 1]),
			     x, y, 2, teal);
		}
		Color c = temperature[i] > LIMIT ? amber : teal;
		HPDF_Page_SetRGBFill(p, c.r, c.g, c.b);
		HPDF_Page_Circle(p, x, HEIGHT - y, temperature[i] > LIMIT ? 4 : 2.5f);
		HPDF_Page_Fill(p);
		if (i % 6 == 0 || i == COUNT - 1) {
			char label[8];
			snprintf(label, sizeof(label), "%02u:%02u", 8 + (unsigned)i / 2,
				 (unsigned)(i % 2) * 30);
			text(p, x - 12, 582, 8, regular, muted, label);
		}
	}
	text(p, 76, 604, 8, regular, muted, "TEAL  Temperature readings       AMBER  Above-threshold region / reading");
	text(p, 36, 653, 17, bold, navy, "Reading the session");
	box(p, 36, 670, 3, 71, teal);
	snprintf(detail, sizeof(detail), "Peak: %d.%d C at 14:00.", maximum / 10, maximum % 10);
	text(p, 50, 686, 12, bold, navy, detail);
	text(p, 50, 707, 10, regular, muted, "A gradual rise is followed by a return toward the morning baseline.");
	text(p, 50, 726, 10, regular, muted, "One reading exceeds the illustrative limit. See the complete log on page 2.");
	text(p, 36, 759, 8, regular, muted, "DEMONSTRATION ONLY  /  No physical sensor, calibration or compliance claim.");
}

static void readings(HPDF_Page p)
{
	text(p, 36, 118, 32, bold, navy, "The measurement log.");
	text(p, 36, 144, 11, regular, muted, "Complete dataset / temperature, relative humidity and threshold status");
	box(p, 36, 167, 523, 30, navy);
	text(p, 49, 186, 9, bold, white, "NO.");
	text(p, 100, 186, 9, bold, white, "LOCAL TIME");
	text(p, 220, 186, 9, bold, white, "TEMP. / C");
	text(p, 330, 186, 9, bold, white, "RH / %");
	text(p, 432, 186, 9, bold, white, "STATUS");
	for (size_t i = 0; i < COUNT; i++) {
		float top = 197 + i * 20;
		char s[24];
		int alert = temperature[i] > LIMIT;

		box(p, 36, top, 523, 20, alert ? warm : (i % 2 ? paper : white));
		snprintf(s, sizeof(s), "%02u", (unsigned)i + 1);
		text(p, 49, top + 14, 9, regular, muted, s);
		snprintf(s, sizeof(s), "%02u:%02u", 8 + (unsigned)i / 2, (unsigned)(i % 2) * 30);
		text(p, 100, top + 14, 10, regular, navy, s);
		decimal(s, sizeof(s), temperature[i]);
		text(p, 220, top + 14, 10, bold, alert ? amber : navy, s);
		snprintf(s, sizeof(s), "%d", humidity[i]);
		text(p, 330, top + 14, 10, regular, navy, s);
		text(p, 432, top + 14, 8, bold, alert ? amber : teal,
		     alert ? "ABOVE LIMIT" : "WITHIN LIMIT");
	}
	line(p, 36, 678, 559, 678, 0.5f, muted);
	text(p, 36, 706, 10, bold, navy, "METHOD & INTERPRETATION");
	text(p, 36, 726, 9, regular, muted, "Fixed demonstration values sampled every 30 minutes. RH = relative humidity.");
	text(p, 36, 743, 9, regular, muted, "Status compares temperature only: above 25.0 C = above limit; otherwise within limit.");
	text(p, 36, 760, 9, regular, muted, "The threshold is illustrative and is not a safety or calibration specification.");
}

int main(void)
{
	HPDF_Doc pdf = HPDF_New(on_error, NULL);
	int minimum = temperature[0], maximum = temperature[0], sum = 0, alerts = 0;
	int ret = -EIO;
	HPDF_Page first, second;

	if (!pdf) {
		printk("Cannot allocate measurement report\n");
		return -ENOMEM;
	}
	regular = HPDF_GetFont(pdf, "Helvetica", NULL);
	bold = HPDF_GetFont(pdf, "Helvetica-Bold", NULL);
	if (!regular || !bold) {
		goto out;
	}
	HPDF_SetInfoAttr(pdf, HPDF_INFO_TITLE, "Field Notes - Measurement Report");
	HPDF_SetInfoAttr(pdf, HPDF_INFO_AUTHOR, "VIτπ / Zephyr + libharu");
	HPDF_SetInfoAttr(pdf, HPDF_INFO_SUBJECT, "Simulated temperature and humidity measurements");
	for (size_t i = 0; i < COUNT; i++) {
		if (temperature[i] < minimum) minimum = temperature[i];
		if (temperature[i] > maximum) maximum = temperature[i];
		sum += temperature[i];
		alerts += temperature[i] > LIMIT;
	}
	first = page(pdf, 1, "SESSION SUMMARY");
	if (!first) goto out;
	overview(first, minimum, maximum, (sum + COUNT / 2) / COUNT, alerts);
	second = page(pdf, 2, "TRACEABLE READINGS");
	if (!second) goto out;
	readings(second);
	if (!error_code) {
		ret = pdf_output(pdf, "measurements.pdf");
	}
out:
	if (ret) printk("Measurement report failed: 0x%lx\n", (unsigned long)HPDF_GetError(pdf));
	HPDF_Free(pdf);
	return ret;
}
