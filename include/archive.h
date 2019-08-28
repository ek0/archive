#ifndef _ARCHIVE_H_
#define _ARCHIVE_H_

#include "archive_export_config.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define USE_CONSOLE 1

#define ARCHIVE_LOG_TS(fmt, ...) ArchiveLogWithTs("%s: ", __FUNCTION__); \
                                 ArchiveLog(fmt, __VA_ARGS__);

#define ARCHIVE_LOG(fmt, ...) ArchiveLog("%s: ", __FUNCTION__); \
                              ArchiveLog(fmt, __VA_ARGS__);

void ARCHIVE_EXPORT ArchiveInit(int);
void ARCHIVE_EXPORT ArchiveCleanup();
void ARCHIVE_EXPORT ArchiveLog(const char* format, ...);
void ARCHIVE_EXPORT ArchiveLogWithTs(const char* format, ...);
void ARCHIVE_EXPORT ArchiveHexDump(const uint8_t* data, size_t size);

#ifdef __cplusplus
}
#endif

#endif // _ARCHIVE_H_