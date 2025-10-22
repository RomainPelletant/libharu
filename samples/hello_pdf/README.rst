.. SPDX-License-Identifier: Apache-2.0

.. _libharu-hello-pdf:

Hello PDF
#########

Overview
********

This sample creates an A4 page with the text "Hello from Zephyr!" using
Helvetica and sets the document title.

Requirements
************

* A configured Zephyr workspace with the libharu module; see :ref:`libharu`.
* An AArch64 toolchain and QEMU supporting ``qemu_cortex_a53``.
  No physical board is required.

The supplied configuration reserves 50,072 bytes for the malloc arena and
4 KiB for the main stack, in addition to other firmware memory requirements.

Building and Running
********************

From the libharu repository root, build the sample:

.. code-block:: shell

   west build -b qemu_cortex_a53 samples/hello_pdf -d build/hello_pdf

Run the application in QEMU and capture its console output:

.. code-block:: shell

   west build -d build/hello_pdf -t run 2>&1 | tee serial.log

Exit QEMU with :kbd:`Ctrl-a`, then :kbd:`x` after ``PDF-END`` appears.

Sample Output
*************

The console emits ``PDF-BEGIN hello.pdf <size>``, followed by ``PDF-DATA``
lines and a final ``PDF-END`` marker. Capture the console to ``serial.log``
and recover the document from the repository root:

.. code-block:: shell

   python3 samples/capture_pdf.py serial.log hello.pdf

See :ref:`libharu_pdf_capture` for the transfer format and validation checks.

Minimal configuration
*********************

To retain Helvetica while disabling the other built-in font families,
TrueType, password encryption and PDF/A, build with the supplied overlay:

.. code-block:: shell

   west build -b qemu_cortex_a53 samples/hello_pdf -d build/hello-minimal -- -DEXTRA_CONF_FILE=minimal.conf
