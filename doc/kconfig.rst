.. SPDX-License-Identifier: Apache-2.0

.. _libharu_kconfig:

Kconfig configuration
#####################

Set options in your application's ``prj.conf`` or in an extra configuration
file passed with ``-DEXTRA_CONF_FILE=...``. To configure an existing build
interactively, run the following from the repository root:

.. code-block:: shell

   west build -d build/hello_pdf -t menuconfig

For example, a configuration using Helvetica without TrueType, password
encryption or PDF/A can contain:

.. code-block:: ini

   CONFIG_LIBHARU=y
   CONFIG_LIBHARU_FONT_HELVETICA=y
   CONFIG_LIBHARU_FONT_COURIER=n
   CONFIG_LIBHARU_FONT_TIMES=n
   CONFIG_LIBHARU_FONT_SYMBOL=n
   CONFIG_LIBHARU_FONT_ZAPF_DINGBATS=n
   CONFIG_LIBHARU_TRUETYPE=n
   CONFIG_LIBHARU_ENCRYPTION=n
   CONFIG_LIBHARU_PDFA=n

The application must also provide the libc, memory and stack settings
explained in :ref:`libharu`. Disabling TrueType also disables the dependent
UTF-8 and Asian font options.

All component options default to ``y``; file support defaults to ``n``. The
application's generated ``build/hello_pdf/zephyr/.config`` contains the
effective values.

General
*******

.. list-table::
   :header-rows: 1
   :widths: auto

   * - Option
     - Description
   * - ``CONFIG_LIBHARU``
     - Enable the libharu PDF library. Requires ``CONFIG_COMMON_LIBC_MALLOC``
       with ``CONFIG_COMMON_LIBC_MALLOC_ARENA_SIZE`` of at least 30000 bytes.
   * - ``CONFIG_LIBHARU_FILE_SUPPORT``
     - Enable the file APIs (``HPDF_SaveToFile()``, ``*FromFile()``) on top
       of Zephyr's file system API. Paths are mount paths such as
       ``/lfs/report.pdf``. Requires ``CONFIG_FILE_SYSTEM``.

Built-in fonts
**************

.. list-table::
   :header-rows: 1
   :widths: auto

   * - Option
     - Description
   * - ``CONFIG_LIBHARU_FONT_HELVETICA``
     - Helvetica and its bold, oblique and bold-oblique variants.
   * - ``CONFIG_LIBHARU_FONT_COURIER``
     - Courier and its bold, oblique and bold-oblique variants.
   * - ``CONFIG_LIBHARU_FONT_TIMES``
     - Times-Roman and its bold, italic and bold-italic variants.
   * - ``CONFIG_LIBHARU_FONT_SYMBOL``
     - Symbol font.
   * - ``CONFIG_LIBHARU_FONT_ZAPF_DINGBATS``
     - ZapfDingbats font.

A disabled family behaves like an unknown font: ``HPDF_GetFont()`` returns
NULL with ``HPDF_INVALID_FONT_NAME``.

TrueType and composite fonts
****************************

.. list-table::
   :header-rows: 1
   :widths: auto

   * - Option
     - Description
   * - ``CONFIG_LIBHARU_TRUETYPE``
     - Load TrueType fonts from memory or files, and the shared Type0/CID font
       engine. Required by every option below.
   * - ``CONFIG_LIBHARU_ENCODING_UTF``
     - Enable ``HPDF_UseUTFEncodings()``. Requires a suitable TrueType font.
   * - ``CONFIG_LIBHARU_FONT_JP``
     - Japanese fonts and encodings (``HPDF_UseJPFonts()``,
       ``HPDF_UseJPEncodings()``).
   * - ``CONFIG_LIBHARU_FONT_KR``
     - Korean fonts and encodings (``HPDF_UseKRFonts()``,
       ``HPDF_UseKREncodings()``).
   * - ``CONFIG_LIBHARU_FONT_CNS``
     - Simplified Chinese fonts and encodings (``HPDF_UseCNSFonts()``,
       ``HPDF_UseCNSEncodings()``).
   * - ``CONFIG_LIBHARU_FONT_CNT``
     - Traditional Chinese fonts and encodings (``HPDF_UseCNTFonts()``,
       ``HPDF_UseCNTEncodings()``).

Encryption and PDF/A
********************

.. list-table::
   :header-rows: 1
   :widths: auto

   * - Option
     - Description
   * - ``CONFIG_LIBHARU_ENCRYPTION``
     - Password, permission and encryption-mode APIs.
   * - ``CONFIG_LIBHARU_PDFA``
     - PDF/A conformance, XMP extensions and output intents. Keeps the shared
       MD5 implementation for document identifiers, even when encryption is
       disabled.
