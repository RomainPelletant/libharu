.. SPDX-License-Identifier: Apache-2.0

.. _libharu-report:

PDF sensor report
#################

Overview
********

This sample creates three A4 pages containing 60 simulated temperature
readings. It uses Courier for the tabular text and adds a document title.

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

   west build -b qemu_cortex_a53 samples/report -d build/report

Run the application in QEMU and capture its console output:

.. code-block:: shell

   west build -d build/report -t run 2>&1 | tee serial.log

Exit QEMU with :kbd:`Ctrl-a`, then :kbd:`x` after ``PDF-END`` appears.

Sample Output
*************

The console emits ``PDF-BEGIN report.pdf <size>``, followed by ``PDF-DATA``
lines and a final ``PDF-END`` marker. Capture the console to ``serial.log``
and recover the document from the repository root:

.. code-block:: shell

   python3 samples/capture_pdf.py serial.log report.pdf

See :ref:`libharu_pdf_capture` for the transfer format and validation checks.
