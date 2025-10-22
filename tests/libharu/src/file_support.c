/* SPDX-License-Identifier: Apache-2.0 */
#include <hpdf.h>
#include <zephyr/fs/fs.h>
/* After fs.h: ext2.h uses bool without including stdbool.h. */
#include <zephyr/fs/ext2.h>
#include <zephyr/ztest.h>

#define MOUNT_POINT "/ram"

static struct fs_mount_t mount = {
	.type = FS_EXT2,
	.mnt_point = MOUNT_POINT,
	.storage_dev = "RAM",
	.flags = FS_MOUNT_FLAG_NO_FORMAT,
};

static HPDF_Doc pdf;

static void *setup(void)
{
	/* The ext2 driver logs the UUID as a string: keep it printable. */
	static struct ext2_cfg cfg = {
		.block_size = 1024,
		.uuid = "libharu-test-fs",
		.volume_name = "libharu",
		.set_uuid = true,
	};

	zassert_ok(fs_mkfs(FS_EXT2, (uintptr_t)mount.storage_dev, &cfg, 0));
	zassert_ok(fs_mount(&mount));
	return NULL;
}

static void teardown(void *fixture)
{
	ARG_UNUSED(fixture);
	fs_unmount(&mount);
}

static void before(void *fixture)
{
	ARG_UNUSED(fixture);
	pdf = HPDF_New(NULL, NULL);
	zassert_not_null(pdf);
}

static void after(void *fixture)
{
	ARG_UNUSED(fixture);
	HPDF_Free(pdf);
	pdf = NULL;
}

ZTEST(file_support, test_save_to_file)
{
	struct fs_dirent entry;
	struct fs_file_t file;
	char header[5];
	HPDF_UINT32 expected;

	fs_file_t_init(&file);
	zassert_not_null(HPDF_AddPage(pdf));
	zassert_equal(HPDF_SaveToStream(pdf), HPDF_OK);
	expected = HPDF_GetStreamSize(pdf);

	zassert_equal(HPDF_SaveToFile(pdf, MOUNT_POINT "/out.pdf"), HPDF_OK);
	zassert_ok(fs_stat(MOUNT_POINT "/out.pdf", &entry));
	zassert_equal(entry.size, expected, "file has %zu bytes, stream %u", entry.size,
		      expected);

	zassert_ok(fs_open(&file, MOUNT_POINT "/out.pdf", FS_O_READ));
	zassert_equal(fs_read(&file, header, sizeof(header)), sizeof(header));
	zassert_ok(fs_close(&file));
	zassert_mem_equal(header, "%PDF-", sizeof(header));
}

ZTEST(file_support, test_load_raw_image_from_file)
{
	static const HPDF_BYTE pixels[] = {255, 0, 0, 0, 255, 0};
	struct fs_file_t file;
	HPDF_Image image;

	fs_file_t_init(&file);
	zassert_ok(fs_open(&file, MOUNT_POINT "/image.rgb", FS_O_CREATE | FS_O_WRITE));
	zassert_equal(fs_write(&file, pixels, sizeof(pixels)), sizeof(pixels));
	zassert_ok(fs_close(&file));

	image = HPDF_LoadRawImageFromFile(pdf, MOUNT_POINT "/image.rgb", 2, 1,
					  HPDF_CS_DEVICE_RGB);
	zassert_not_null(image);
	zassert_equal(HPDF_Image_GetWidth(image), 2);
	zassert_equal(HPDF_Image_GetHeight(image), 1);
}

ZTEST(file_support, test_missing_file)
{
	zassert_is_null(HPDF_LoadRawImageFromFile(pdf, MOUNT_POINT "/missing.rgb", 2, 1,
						  HPDF_CS_DEVICE_RGB));
	zassert_equal(HPDF_GetError(pdf), HPDF_FILE_OPEN_ERROR);
}

ZTEST_SUITE(file_support, NULL, setup, before, after, teardown);
