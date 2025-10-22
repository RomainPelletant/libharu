.. SPDX-License-Identifier: Apache-2.0

.. _libharu_samples:

Libharu samples
###############

These samples generate complete PDF documents with the standard libharu API.
See :ref:`libharu` for module configuration and workspace requirements.

* :ref:`libharu-hello-pdf`
* :ref:`libharu-report`
* :ref:`libharu-measurement-report`
* :ref:`libharu-graphics`
* :ref:`libharu-encrypted-pdf`

.. _libharu_pdf_capture:

Recovering a PDF
****************

Each sample writes its PDF to the console as hexadecimal data. No filesystem,
SD card or binary-safe console transfer is required. The reference target is
``qemu_cortex_a53``. Capture its emulated console to ``serial.log`` with the
run command shown in each sample guide. No physical UART or baud-rate setup
is needed.

The output has the following structure. ``<size>`` is the number of PDF bytes;
``<hexadecimal data>`` represents the data rather than literal console text:

.. code-block:: text

   PDF-BEGIN hello.pdf <size>
   PDF-DATA <hexadecimal data>
   ...
   PDF-END

From the libharu repository root, extract the first complete transfer:

.. code-block:: shell

   python3 samples/capture_pdf.py serial.log hello.pdf

Choose a new output filename for each extraction. The script checks framing,
byte count, PDF header and trailer, and refuses to overwrite an existing file.
Use ``report.pdf``, ``graphics.pdf`` or ``encrypted.pdf`` for the other samples.

The shared output helper reads the memory stream in 32-byte chunks.
Hexadecimal encoding doubles the transmitted byte count. Avoid interleaving
other log messages inside ``PDF-DATA`` lines. A generation failure does not
emit ``PDF-END`` and must not be treated as a complete transfer.
