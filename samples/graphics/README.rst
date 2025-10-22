.. SPDX-License-Identifier: Apache-2.0

.. _libharu-graphics:

PDF graphics
############

Overview
********

This sample draws a five-bar chart and embeds a 2 by 2 RGB image enlarged
to show its red, green, blue and yellow pixels. It requires no external image
file or PNG decoder.

Requirements
************

* A configured Zephyr workspace with the libharu module; see :ref:`libharu`.
* An AArch64 toolchain and QEMU supporting ``qemu_cortex_a53``.
  No physical board is required.

The supplied configuration reserves 128 KiB for the malloc arena and
4 KiB for the main stack, in addition to other firmware memory requirements.

Building and Running
********************

From the libharu repository root, build the sample:

.. code-block:: shell

   west build -b qemu_cortex_a53 samples/graphics -d build/graphics

Run the application in QEMU and capture its console output:

.. code-block:: shell

   west build -d build/graphics -t run 2>&1 | tee serial.log

Exit QEMU with :kbd:`Ctrl-a`, then :kbd:`x` after ``PDF-END`` appears.

Sample Output
*************

The console emits ``PDF-BEGIN graphics.pdf <size>``, followed by ``PDF-DATA``
lines and a final ``PDF-END`` marker. Capture the console to ``serial.log``
and recover the document from the repository root:

.. code-block:: shell

   python3 samples/capture_pdf.py serial.log graphics.pdf

See :ref:`libharu_pdf_capture` for the transfer format and validation checks.
