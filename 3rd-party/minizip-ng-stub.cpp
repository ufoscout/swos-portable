/*
 * Stub implementation of minizip-ng functions for platforms where
 * the minizip-ng library is not available as a system package.
 * All functions return failure so zip-based features (e.g. commentary
 * audio from zip archives) are silently unavailable.
 */
#include <cstdint>
#include <cstdlib>

#include <mz.h>
#include <mz_zip.h>
#include <mz_strm.h>
#include <mz_zip_rw.h>

void *mz_zip_reader_create(void)          { return nullptr; }
void  mz_zip_reader_delete(void **handle) { (void)handle; }

int32_t mz_zip_reader_open_buffer(void *handle, uint8_t *buf, int32_t len, uint8_t copy)
    { (void)handle; (void)buf; (void)len; (void)copy; return MZ_SUPPORT_ERROR; }

int32_t mz_zip_reader_goto_first_entry(void *handle)
    { (void)handle; return MZ_SUPPORT_ERROR; }

int32_t mz_zip_reader_goto_next_entry(void *handle)
    { (void)handle; return MZ_SUPPORT_ERROR; }

int32_t mz_zip_reader_entry_get_info(void *handle, mz_zip_file **file_info)
    { (void)handle; (void)file_info; return MZ_SUPPORT_ERROR; }

int32_t mz_zip_reader_entry_is_dir(void *handle)
    { (void)handle; return MZ_SUPPORT_ERROR; }

int32_t mz_zip_reader_entry_open(void *handle)
    { (void)handle; return MZ_SUPPORT_ERROR; }

int32_t mz_zip_reader_entry_read(void *handle, void *buf, int32_t len)
    { (void)handle; (void)buf; (void)len; return MZ_SUPPORT_ERROR; }

int32_t mz_zip_reader_entry_close(void *handle)
    { (void)handle; return MZ_SUPPORT_ERROR; }
