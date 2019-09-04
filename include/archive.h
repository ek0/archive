#ifndef _ARCHIVE_H_
#define _ARCHIVE_H_

#include "archive_export_config.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define ARCHIVE_SUCCESS                     0x00000000
#define ARCHIVE_ERROR_CONSOLE               0x00000001
#define ARCHIVE_ERROR_NOT_INITIALIZED       0x00000002
#define ARCHIVE_ERROR_ALLOCATING            0x00000003
#define ARCHIVE_ERROR_INVALID_FILE_HANDLE   0x00000004

#define ARCHIVE_USE_CONSOLE 0x00000001
#define ARCHIVE_NO_LOG_FILE 0x00000002

#define ARCHIVE_LOG_TS(fmt, ...) ArchiveLogWithTs("%s: ", __FUNCTION__); \
                                 ArchiveLog(fmt, __VA_ARGS__);

#define ARCHIVE_LOG(fmt, ...) ArchiveLog("%s: ", __FUNCTION__); \
                              ArchiveLog(fmt, __VA_ARGS__);

int ARCHIVE_EXPORT  ArchiveInit(int, const char*);
void ARCHIVE_EXPORT ArchiveCleanup();
int ARCHIVE_EXPORT  ArchiveLog(const char* format, ...);
int ARCHIVE_EXPORT  ArchiveLogWithTs(const char* format, ...);
int ARCHIVE_EXPORT  ArchiveHexDump(const uint8_t* data, size_t size);

#ifdef __cplusplus
}
#endif

#endif // _ARCHIVE_H_