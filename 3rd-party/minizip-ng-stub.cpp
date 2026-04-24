/*
 * Adapter implementing the minizip-ng 2.x API using the minizip 1.x library
 * (libminizip-dev) available on Ubuntu 24.04 and earlier.
 *
 * The zip reader is opened from a memory buffer via a custom zlib_filefunc_def
 * that satisfies minizip 1.x's I/O abstraction without touching the filesystem.
 */
#include <cstdint>
#include <cstring>

#include <minizip/unzip.h>
#include <minizip/ioapi.h>

#include <mz.h>
#include <mz_zip.h>
#include <mz_strm.h>
#include <mz_zip_rw.h>

struct ZipReaderState {
    uint8_t     *buf;
    int32_t      len;
    int32_t      pos;
    bool         ownsBuf;
    unzFile      unz;
    mz_zip_file  currentFileInfo;
    char         currentFilename[512];
    bool         hasCurrentEntry;
};

// --- in-memory I/O callbacks for zlib_filefunc_def ---

static voidpf mem_open(voidpf opaque, const char *, int)
{
    return opaque;  // stream == state
}

static uLong mem_read(voidpf, voidpf stream, void *buf, uLong size)
{
    auto *s = static_cast<ZipReaderState *>(stream);
    uLong remaining = static_cast<uLong>(s->len - s->pos);
    if (size > remaining)
        size = remaining;
    memcpy(buf, s->buf + s->pos, size);
    s->pos += static_cast<int32_t>(size);
    return size;
}

static uLong mem_write(voidpf, voidpf, const void *, uLong) { return 0; }

static long mem_tell(voidpf, voidpf stream)
{
    return static_cast<ZipReaderState *>(stream)->pos;
}

static long mem_seek(voidpf, voidpf stream, uLong offset, int origin)
{
    auto *s = static_cast<ZipReaderState *>(stream);
    switch (origin) {
        case ZLIB_FILEFUNC_SEEK_SET: s->pos = static_cast<int32_t>(offset); break;
        case ZLIB_FILEFUNC_SEEK_CUR: s->pos += static_cast<int32_t>(offset); break;
        case ZLIB_FILEFUNC_SEEK_END: s->pos = s->len + static_cast<int32_t>(offset); break;
    }
    return 0;
}

static int mem_close(voidpf, voidpf) { return 0; }
static int mem_error(voidpf, voidpf) { return 0; }

// --- minizip-ng 2.x API implementation ---

void *mz_zip_reader_create(void)
{
    return new ZipReaderState{};
}

void mz_zip_reader_delete(void **handle)
{
    if (!handle || !*handle)
        return;
    auto *s = static_cast<ZipReaderState *>(*handle);
    if (s->unz)
        unzClose(s->unz);
    if (s->ownsBuf)
        delete[] s->buf;
    delete s;
    *handle = nullptr;
}

int32_t mz_zip_reader_open_buffer(void *handle, uint8_t *buf, int32_t len, uint8_t copy)
{
    auto *s = static_cast<ZipReaderState *>(handle);

    if (copy) {
        s->buf = new uint8_t[len];
        memcpy(s->buf, buf, len);
        s->ownsBuf = true;
    } else {
        s->buf = buf;
        s->ownsBuf = false;
    }
    s->len = len;
    s->pos = 0;

    zlib_filefunc_def filefunc{};
    filefunc.zopen_file  = mem_open;
    filefunc.zread_file  = mem_read;
    filefunc.zwrite_file = mem_write;
    filefunc.ztell_file  = mem_tell;
    filefunc.zseek_file  = mem_seek;
    filefunc.zclose_file = mem_close;
    filefunc.zerror_file = mem_error;
    filefunc.opaque      = s;

    s->unz = unzOpen2(nullptr, &filefunc);
    return s->unz ? MZ_OK : MZ_OPEN_ERROR;
}

static int32_t fillCurrentFileInfo(ZipReaderState *s)
{
    unz_file_info64 info{};
    int r = unzGetCurrentFileInfo64(s->unz, &info,
        s->currentFilename, sizeof(s->currentFilename) - 1,
        nullptr, 0, nullptr, 0);
    if (r != UNZ_OK)
        return MZ_READ_ERROR;

    memset(&s->currentFileInfo, 0, sizeof(s->currentFileInfo));
    s->currentFileInfo.filename       = s->currentFilename;
    s->currentFileInfo.filename_size  = static_cast<uint16_t>(strlen(s->currentFilename));
    s->currentFileInfo.uncompressed_size = static_cast<int64_t>(info.uncompressed_size);
    s->hasCurrentEntry = true;
    return MZ_OK;
}

int32_t mz_zip_reader_goto_first_entry(void *handle)
{
    auto *s = static_cast<ZipReaderState *>(handle);
    if (unzGoToFirstFile(s->unz) != UNZ_OK)
        return MZ_END_OF_LIST;
    return fillCurrentFileInfo(s);
}

int32_t mz_zip_reader_goto_next_entry(void *handle)
{
    auto *s = static_cast<ZipReaderState *>(handle);
    int r = unzGoToNextFile(s->unz);
    if (r == UNZ_END_OF_LIST_OF_FILE)
        return MZ_END_OF_LIST;
    if (r != UNZ_OK)
        return MZ_READ_ERROR;
    return fillCurrentFileInfo(s);
}

int32_t mz_zip_reader_entry_get_info(void *handle, mz_zip_file **file_info)
{
    auto *s = static_cast<ZipReaderState *>(handle);
    if (!s->hasCurrentEntry)
        return MZ_EXIST_ERROR;
    *file_info = &s->currentFileInfo;
    return MZ_OK;
}

int32_t mz_zip_reader_entry_is_dir(void *handle)
{
    auto *s = static_cast<ZipReaderState *>(handle);
    if (!s->hasCurrentEntry)
        return MZ_EXIST_ERROR;
    uint16_t len = s->currentFileInfo.filename_size;
    // directory entries have a trailing slash
    return (len > 0 && s->currentFilename[len - 1] == '/') ? MZ_OK : MZ_EXIST_ERROR;
}

int32_t mz_zip_reader_entry_open(void *handle)
{
    auto *s = static_cast<ZipReaderState *>(handle);
    return unzOpenCurrentFile(s->unz) == UNZ_OK ? MZ_OK : MZ_OPEN_ERROR;
}

int32_t mz_zip_reader_entry_read(void *handle, void *buf, int32_t len)
{
    auto *s = static_cast<ZipReaderState *>(handle);
    return unzReadCurrentFile(s->unz, buf, static_cast<unsigned>(len));
}

int32_t mz_zip_reader_entry_close(void *handle)
{
    auto *s = static_cast<ZipReaderState *>(handle);
    return unzCloseCurrentFile(s->unz) == UNZ_OK ? MZ_OK : MZ_CLOSE_ERROR;
}
