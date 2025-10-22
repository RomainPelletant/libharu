.. SPDX-License-Identifier: Apache-2.0

.. _libharu:

Libharu
#######

Overview
********

Libharu generates PDF documents on Zephyr using the standard ``hpdf.h`` API.
The module supports in-memory documents and streams, built-in fonts, TrueType
fonts, vector graphics, raw images, password encryption and PDF/A metadata.

See :ref:`libharu_samples` for complete PDF generation examples.

Requirements
************

Use a Zephyr workspace containing this module and the dependencies listed in
its west manifest. With this repository configured as the manifest repository,
update the workspace and register Zephyr:

.. code-block:: shell

   west update
   west zephyr-export

Install the Zephyr Python requirements and a toolchain for the target board.
The samples register this checkout through ``ZEPHYR_EXTRA_MODULES``.
The reference target is ``qemu_cortex_a53``. Install the AArch64 toolchain
and QEMU support; no physical board is required.

Configuration
*************

Enable ``CONFIG_LIBHARU`` and a C library providing malloc.
The samples use ``CONFIG_PICOLIBC``. The module requires
``CONFIG_COMMON_LIBC_MALLOC_ARENA_SIZE`` to be at least 30,000 bytes;
actual memory requirements depend on the document.

Libharu uses dynamic allocation. It retains document objects and the serialized
PDF in RAM. Reading the stream in chunks does not bound the total document
memory. Release each document with ``HPDF_Free()`` and check API return values.
Applications must synchronize access when sharing a document between threads.

Component selection
===================

Configure components under :menuselection:`Libharu PDF library --> PDF components`
in menuconfig. All of the following options default to enabled:

See :ref:`libharu_kconfig` for the reference generated directly from the
module Kconfig, including help text, defaults and dependencies.

UTF-8 and the Asian font groups depend on TrueType support. PDF/A retains the
shared MD5 implementation even when password encryption is disabled.

Guard optional API calls with the corresponding ``CONFIG_LIBHARU_*`` symbol.
Declarations remain visible: calls into excluded implementations can fail to
link, while core entry points backed by disabled components return errors.
Requesting a disabled built-in font reports ``HPDF_INVALID_FONT_NAME``.

File and image support
======================

The samples use memory streams and do not require a filesystem. File APIs
(``HPDF_SaveToFile()`` and the ``*FromFile()`` loaders) require
``CONFIG_LIBHARU_FILE_SUPPORT`` and ``CONFIG_FILE_SYSTEM``, with the
filesystem mounted by the application. File support defaults to disabled.

On Zephyr, libharu does not use the C library's stdio for files:
``zephyr/src/hpdf_zephyr_fs.c`` maps its file macros to Zephyr's file system
API (``fs_open()``, ``fs_read()``, ...), so no POSIX layer is needed. Paths are
Zephyr mount paths, for example ``/lfs/report.pdf``.

PNG and zlib are disabled by the Zephyr module build. Raw RGB images are
available without either dependency.

Testing
*******

Run the following command from the libharu repository root to compile all
sample and test configurations without flashing or executing them:

.. code-block:: shell

   west twister -T samples -T tests -p qemu_cortex_a53 --build-only --outdir build/twister

The tests cover document lifecycle, metadata, stream serialization, error
handling, raw images and optional components. Configurations include default,
minimal, no built-in fonts, TrueType/UTF-8 without Asian tables, and PDF/A
without encryption.

The test configuration reserves a 512 KiB malloc arena and additional test
stack space. A composite encoder alone allocates two 256 by 256 16-bit maps
(256 KiB). Build-only validation does not establish runtime test results.
