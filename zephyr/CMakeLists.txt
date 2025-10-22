# SPDX-License-Identifier: Apache-2.0

if(NOT CONFIG_LIBHARU)
  return()
endif()

set(LIBHARU_DIR ${CMAKE_CURRENT_LIST_DIR}/..)

# PNG and zlib are optional desktop dependencies, not Zephyr host libraries.
set(LIBHPDF_HAVE_LIBPNG OFF)
set(LIBHPDF_HAVE_ZLIB OFF)
configure_file(${LIBHARU_DIR}/include/hpdf_config.h.cmake
               ${CMAKE_CURRENT_BINARY_DIR}/include/hpdf_config.h)
zephyr_include_directories(${LIBHARU_DIR}/include
                           ${CMAKE_CURRENT_LIST_DIR}/include
                           ${CMAKE_CURRENT_BINARY_DIR}/include)

# Every upstream source is a library source. Components disabled in Kconfig
# drop their files here; the internal headers replace their entry points with
# inline stubs so the remaining sources build unchanged.
file(GLOB LIBHARU_SRCS ${LIBHARU_DIR}/src/*.c)

function(libharu_exclude)
  list(TRANSFORM ARGN PREPEND ${LIBHARU_DIR}/src/)
  list(REMOVE_ITEM LIBHARU_SRCS ${ARGN})
  set(LIBHARU_SRCS ${LIBHARU_SRCS} PARENT_SCOPE)
endfunction()

if(NOT CONFIG_LIBHARU_TRUETYPE)
  libharu_exclude(hpdf_font_tt.c hpdf_fontdef_tt.c hpdf_font_cid.c hpdf_fontdef_cid.c)
endif()
if(NOT CONFIG_LIBHARU_ENCODING_UTF)
  libharu_exclude(hpdf_encoder_utf.c)
endif()
if(NOT CONFIG_LIBHARU_ENCRYPTION)
  libharu_exclude(hpdf_encryptdict.c)
endif()
if(NOT CONFIG_LIBHARU_ENCRYPTION AND NOT CONFIG_LIBHARU_PDFA)
  # PDF/A reuses the MD5 implementation for document identifiers.
  libharu_exclude(hpdf_encrypt.c)
endif()
if(NOT CONFIG_LIBHARU_PDFA)
  libharu_exclude(hpdf_pdfa.c)
endif()
if(NOT CONFIG_LIBHARU_FONT_JP)
  libharu_exclude(hpdf_encoder_jp.c hpdf_fontdef_jp.c)
endif()
if(NOT CONFIG_LIBHARU_FONT_KR)
  libharu_exclude(hpdf_encoder_kr.c hpdf_fontdef_kr.c)
endif()
if(NOT CONFIG_LIBHARU_FONT_CNS)
  libharu_exclude(hpdf_encoder_cns.c hpdf_fontdef_cns.c)
endif()
if(NOT CONFIG_LIBHARU_FONT_CNT)
  libharu_exclude(hpdf_encoder_cnt.c hpdf_fontdef_cnt.c)
endif()

zephyr_library()
zephyr_library_sources(${LIBHARU_SRCS})
# File I/O back end over Zephyr's fs API (replaces stdio, see hpdf_conf.h).
zephyr_library_sources_ifdef(CONFIG_LIBHARU_FILE_SUPPORT
                             ${CMAKE_CURRENT_LIST_DIR}/src/hpdf_zephyr_fs.c)
# Upstream code style; unused built-in font tables are discarded by the compiler.
zephyr_library_compile_options(-Wno-double-promotion -Wno-unused-function
                               -Wno-unused-const-variable)
