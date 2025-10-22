.. SPDX-License-Identifier: Apache-2.0

.. _libharu-encrypted-pdf:

Password-protected PDF
######################

Overview
********

This sample creates an A4 document using Helvetica and the libharu password
and permission APIs. It enables encryption and disables unused optional
components, including TrueType and PDF/A.

Requirements
************

* A configured Zephyr workspace with the libharu module; see :ref:`libharu`.
* An AArch64 toolchain and QEMU supporting ``qemu_cortex_a53``.
  No physical board is required.

The supplied configuration reserves 40,072 bytes for the malloc arena and
16 KiB for the main stack, in addition to other firmware memory requirements.

Building and Running
********************

From the libharu repository root, build the sample:

.. code-block:: shell

   west build -b qemu_cortex_a53 samples/encrypted_pdf -d build/encrypted_pdf

Run the application in QEMU and capture its console output:

.. code-block:: shell

   west build -d build/encrypted_pdf -t run 2>&1 | tee serial.log

Exit QEMU with :kbd:`Ctrl-a`, then :kbd:`x` after ``PDF-END`` appears.

Sample Output
*************

The console emits ``PDF-BEGIN encrypted.pdf <size>``, followed by ``PDF-DATA``
lines and a final ``PDF-END`` marker. Capture the console to ``serial.log``
and recover the document from the repository root:

.. code-block:: shell

   python3 samples/capture_pdf.py serial.log encrypted.pdf

See :ref:`libharu_pdf_capture` for the transfer format and validation checks.

Passwords and permissions
*************************

Open the PDF with the reader password ``zephyr-reader``. The owner password
is ``zephyr-owner``. These fixed credentials are for demonstration only.
The sample allows reading and printing; PDF viewers enforce the restrictions
on copying and modification.

The sample calls ``HPDF_SetPassword()`` before ``HPDF_SetEncryptionMode()`` and
``HPDF_SetPermission()`` to create the encryption dictionary first.
``HPDF_ENCRYPT_R3`` with a 16-byte key selects legacy RC4-128 encryption,
not AES. It is unsuitable for modern confidentiality requirements.
The console transfer contains the encrypted PDF encoded as hexadecimal.

Serialization needs input and encrypted-output buffers on the stack, which
is why this sample reserves a larger main stack.
