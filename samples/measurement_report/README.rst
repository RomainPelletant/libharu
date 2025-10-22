.. SPDX-License-Identifier: Apache-2.0

.. _libharu-measurement-report:

Measurement report
##################

Overview
********

Generate a styled, two-page A4 report using only the standard libharu API:

* Session summary, mean temperature, observed range and exceedance count.
* Vector temperature chart with grid, time axis and an illustrative threshold.
* Complete 24-row temperature/humidity table with alternating rows and alerts.
* Repeated page identity, page numbering and document metadata.

The deterministic data represents a simulated session from 08:00 to 19:30,
with readings every 30 minutes. Temperature is stored in tenths of a degree
Celsius and humidity in percent. Statistics are calculated from the dataset.
The 25.0 C threshold is illustrative; this is not a safety assessment.

Requirements
************

Use a Zephyr workspace with libharu, an AArch64 toolchain and QEMU.
The configuration reserves a 192 KiB malloc arena and an 8 KiB main stack.
Only the Helvetica font family is enabled; external fonts, images,
filesystems, PNG and compression are not needed.

Building and Running
********************

From the repository root:

.. code-block:: shell

   west build -b qemu_cortex_a53 samples/measurement_report -d build/measurement_report
   west build -d build/measurement_report -t run 2>&1 | tee serial.log

After ``PDF-END``, exit QEMU with :kbd:`Ctrl-a`, then :kbd:`x`, and extract
the PDF to a new file:

.. code-block:: shell

   python3 samples/capture_pdf.py serial.log measurements.pdf

See :ref:`libharu_pdf_capture` for the console framing. The report uses vector
text, lines and shapes, so it remains sharp when zoomed or printed.

Customization
*************

The sample's ``src/main.c`` contains the dataset, colors, layout helpers and
page drawing functions. Coordinates are expressed from the top-left corner
in PDF points. Change the session metadata, dataset and narrative together;
the chart scale is fixed at 20 to 26 C and the layout fits 24 readings.
For other ranges or longer sessions, adjust the axes and paginate the table.
