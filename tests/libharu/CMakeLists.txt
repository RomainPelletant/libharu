# SPDX-License-Identifier: Apache-2.0
cmake_minimum_required(VERSION 3.20.0)
list(APPEND ZEPHYR_EXTRA_MODULES ${CMAKE_CURRENT_SOURCE_DIR}/../..)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(libharu_tests)
target_sources(app PRIVATE src/main.c src/components.c src/memory.c)
target_sources_ifdef(CONFIG_LIBHARU_FILE_SUPPORT app PRIVATE src/file_support.c)
