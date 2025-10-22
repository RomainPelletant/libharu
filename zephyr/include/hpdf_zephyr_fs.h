/* SPDX-License-Identifier: Apache-2.0 */
#ifndef HPDF_ZEPHYR_FS_H_
#define HPDF_ZEPHYR_FS_H_

/*
 * File back end for libharu on Zephyr, included by hpdf_conf.h.
 *
 * libharu performs all file I/O through the HPDF_F* macros. With
 * CONFIG_LIBHARU_FILE_SUPPORT they are routed to Zephyr's file system API
 * instead of the C library's stdio, which needs no POSIX layer. Paths are
 * Zephyr mount paths, for example "/lfs/report.pdf".
 */

#if defined(CONFIG_LIBHARU_FILE_SUPPORT)

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

struct hpdf_zephyr_file;

struct hpdf_zephyr_file *hpdf_zephyr_fopen(const char *path, const char *mode);
int hpdf_zephyr_fclose(struct hpdf_zephyr_file *fp);
size_t hpdf_zephyr_fread(void *ptr, size_t size, size_t count, struct hpdf_zephyr_file *fp);
size_t hpdf_zephyr_fwrite(const void *ptr, size_t size, size_t count,
			  struct hpdf_zephyr_file *fp);
int hpdf_zephyr_fflush(struct hpdf_zephyr_file *fp);
int hpdf_zephyr_fseek(struct hpdf_zephyr_file *fp, long offset, int whence);
long hpdf_zephyr_ftell(struct hpdf_zephyr_file *fp);
int hpdf_zephyr_feof(struct hpdf_zephyr_file *fp);
int hpdf_zephyr_ferror(struct hpdf_zephyr_file *fp);

#ifdef __cplusplus
}
#endif

#undef HPDF_FOPEN
#undef HPDF_FCLOSE
#undef HPDF_FREAD
#undef HPDF_FWRITE
#undef HPDF_FFLUSH
#undef HPDF_FSEEK
#undef HPDF_FTELL
#undef HPDF_FEOF
#undef HPDF_FERROR
#undef HPDF_FILEP

#define HPDF_FOPEN  hpdf_zephyr_fopen
#define HPDF_FCLOSE hpdf_zephyr_fclose
#define HPDF_FREAD  hpdf_zephyr_fread
#define HPDF_FWRITE hpdf_zephyr_fwrite
#define HPDF_FFLUSH hpdf_zephyr_fflush
#define HPDF_FSEEK  hpdf_zephyr_fseek
#define HPDF_FTELL  hpdf_zephyr_ftell
#define HPDF_FEOF   hpdf_zephyr_feof
#define HPDF_FERROR hpdf_zephyr_ferror
#define HPDF_FILEP  struct hpdf_zephyr_file *

#endif /* CONFIG_LIBHARU_FILE_SUPPORT */

#endif /* HPDF_ZEPHYR_FS_H_ */
