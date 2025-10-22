/* SPDX-License-Identifier: Apache-2.0 */
/*
 * stdio-like wrappers over Zephyr's file system API for libharu's HPDF_F*
 * macros. Errors are reported as positive errno values: through errno when
 * opening, and through hpdf_zephyr_ferror() afterwards.
 */
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zephyr/fs/fs.h>

#include "hpdf_zephyr_fs.h"

struct hpdf_zephyr_file {
	struct fs_file_t file;
	int error;
	bool eof;
};

struct hpdf_zephyr_file *hpdf_zephyr_fopen(const char *path, const char *mode)
{
	struct hpdf_zephyr_file *fp;
	fs_mode_t flags;
	int ret;

	switch (mode[0]) {
	case 'r':
		flags = FS_O_READ;
		break;
	case 'w':
		flags = FS_O_WRITE | FS_O_CREATE | FS_O_TRUNC;
		break;
	case 'a':
		flags = FS_O_WRITE | FS_O_CREATE | FS_O_APPEND;
		break;
	default:
		errno = EINVAL;
		return NULL;
	}
	if (strchr(mode, '+') != NULL) {
		flags |= FS_O_RDWR;
	}

	/* Same allocator as the rest of libharu's default memory manager. */
	fp = malloc(sizeof(*fp));
	if (fp == NULL) {
		errno = ENOMEM;
		return NULL;
	}
	fs_file_t_init(&fp->file);
	fp->error = 0;
	fp->eof = false;

	ret = fs_open(&fp->file, path, flags);
	if (ret < 0) {
		free(fp);
		errno = -ret;
		return NULL;
	}
	return fp;
}

int hpdf_zephyr_fclose(struct hpdf_zephyr_file *fp)
{
	int ret = fs_close(&fp->file);

	free(fp);
	if (ret < 0) {
		errno = -ret;
		return EOF;
	}
	return 0;
}

size_t hpdf_zephyr_fread(void *ptr, size_t size, size_t count, struct hpdf_zephyr_file *fp)
{
	size_t total = size * count;
	size_t done = 0;

	/* fs_read() may return less than requested before the end of file. */
	while (done < total) {
		ssize_t ret = fs_read(&fp->file, (char *)ptr + done, total - done);

		if (ret < 0) {
			fp->error = (int)-ret;
			break;
		}
		if (ret == 0) {
			fp->eof = true;
			break;
		}
		done += (size_t)ret;
	}
	return size == 0 ? 0 : done / size;
}

size_t hpdf_zephyr_fwrite(const void *ptr, size_t size, size_t count,
			  struct hpdf_zephyr_file *fp)
{
	size_t total = size * count;
	size_t done = 0;

	while (done < total) {
		ssize_t ret = fs_write(&fp->file, (const char *)ptr + done, total - done);

		if (ret < 0) {
			fp->error = (int)-ret;
			break;
		}
		if (ret == 0) {
			/* No progress: the volume is full. */
			fp->error = ENOSPC;
			break;
		}
		done += (size_t)ret;
	}
	return size == 0 ? 0 : done / size;
}

int hpdf_zephyr_fflush(struct hpdf_zephyr_file *fp)
{
	int ret = fs_sync(&fp->file);

	if (ret < 0) {
		fp->error = -ret;
		return EOF;
	}
	return 0;
}

int hpdf_zephyr_fseek(struct hpdf_zephyr_file *fp, long offset, int whence)
{
	int fs_whence;
	int ret;

	switch (whence) {
	case SEEK_CUR:
		fs_whence = FS_SEEK_CUR;
		break;
	case SEEK_END:
		fs_whence = FS_SEEK_END;
		break;
	default:
		fs_whence = FS_SEEK_SET;
		break;
	}

	ret = fs_seek(&fp->file, (off_t)offset, fs_whence);
	if (ret < 0) {
		fp->error = -ret;
		return -1;
	}
	fp->eof = false;
	return 0;
}

long hpdf_zephyr_ftell(struct hpdf_zephyr_file *fp)
{
	off_t pos = fs_tell(&fp->file);

	if (pos < 0) {
		fp->error = (int)-pos;
		return -1;
	}
	return (long)pos;
}

int hpdf_zephyr_feof(struct hpdf_zephyr_file *fp)
{
	return fp->eof;
}

int hpdf_zephyr_ferror(struct hpdf_zephyr_file *fp)
{
	return fp->error;
}
