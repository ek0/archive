
#include "mlist.h"

#include <Windows.h>
#include <stdio.h>
#include <stdint.h>

#include "archive.h"

struct LogFileInfo
{
    LIST_ENTRY next;
    uint32_t   thread_id;
    char       file_path[256];
    HANDLE     file_handle;
};

struct ArchiveHandle
{
    char log_directory[256];
    char current_directory[256];
    CRITICAL_SECTION critical_section;
    int is_critical_section_init;
    int is_init;
    int use_console;
    int use_log_file;
    char buffer[0x10000];
    HANDLE file_handle;
    LIST_ENTRY log_files;
};

static struct ArchiveHandle *archive = NULL;

int ArchiveOpenTemporaryFile()
{
    SYSTEMTIME local_time;

    if(archive == NULL)
        return ARCHIVE_ERROR_NOT_INITIALIZED;
    GetLocalTime(&local_time);
    GetCurrentDirectory(MAX_PATH - 1, archive->current_directory);
    sprintf_s(archive->log_directory,
              256,
              "%s\\%02d-%02d-%04d_%02d-%02d-%02d.txt",
              archive->current_directory,
              local_time.wDay,
              local_time.wMonth,
              local_time.wYear,
              local_time.wHour,
              local_time.wMinute,
              local_time.wSecond);
    return ARCHIVE_SUCCESS;
}

int MakeConsole(void)
{
    DWORD dwMode;
    struct _CONSOLE_SCREEN_BUFFER_INFO sbi;
    HANDLE hStd;
    FILE *stream;

    if (!AllocConsole()) {
        FreeConsole();
        if (!AllocConsole()) {
            return ARCHIVE_ERROR_CONSOLE;
        }
    }
    hStd = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleMode(hStd, (LPDWORD)&dwMode);
    SetConsoleMode(hStd, dwMode & 0xFFFFFFEF);
    GetConsoleScreenBufferInfo(hStd, &sbi);
    sbi.dwSize.Y = 500;
    SetConsoleScreenBufferSize(hStd, sbi.dwSize);
    freopen_s(&stream, "conin$", "r", stdin);
    freopen_s(&stream, "conout$", "w", stdout);
    freopen_s(&stream, "conout$", "w", stderr);
    archive->use_console = 1;
    return ARCHIVE_SUCCESS;
}


/**
 * Open a new file once the session has been previously initialized
 */
void ArchiveOpenNewFile(const char* path)
{
    // TODO
}

int ArchiveSetupLoggingDirectory(const char* file_name)
{
    int status = 0;

    if(archive == NULL)
        return ARCHIVE_ERROR_NOT_INITIALIZED;
    if(file_name == NULL)
    {
        status = ArchiveOpenTemporaryFile();
        if(status != ARCHIVE_SUCCESS)
            return status;
    }
    else
    {
        memcpy(&archive->log_directory, file_name, 256);
    }
    // TODO: Temporary
    archive->file_handle = CreateFile(archive->log_directory,
                                      GENERIC_READ | GENERIC_WRITE,
                                      FILE_SHARE_READ | FILE_SHARE_WRITE,
                                      NULL,
                                      CREATE_ALWAYS,
                                      FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH,
                                      NULL);
    if(archive->file_handle == INVALID_HANDLE_VALUE)
        return ARCHIVE_ERROR_INVALID_FILE_HANDLE;
    archive->use_log_file = 1;
    return ARCHIVE_SUCCESS;
}

int ArchiveInit(int options, const char* file_name)
{
    int status = 0;

    archive = malloc(sizeof(struct ArchiveHandle));
    if(archive == NULL)
        return ARCHIVE_ERROR_NOT_INITIALIZED;
    memset(archive, 0, sizeof(struct ArchiveHandle));
    //InitializeListHead(&archive->log_files);
    if (archive->is_critical_section_init == 0) {
        InitializeCriticalSection(&archive->critical_section);
        archive->is_critical_section_init = 1;
    }
    if(!(options & ARCHIVE_NO_LOG_FILE))
    {
        status = ArchiveSetupLoggingDirectory(file_name);
        if(status != ARCHIVE_SUCCESS)
            return status;
    }
    if(options & ARCHIVE_USE_CONSOLE)
    {
        status = MakeConsole();
        if(status != ARCHIVE_SUCCESS)
            return status;
    }
    archive->is_init = 1;
    return ARCHIVE_SUCCESS;
}

void ArchiveCleanup()
{
    //LIST_ENTRY *entry;
    //struct LogFileInfo *info;

    if(archive != NULL)
    {
        //while(!IsListEmpty(&archive->log_files))
        //{
        //    entry = RemoveHeadList()
        //}
        if(archive->use_log_file)
            CloseHandle(archive->file_handle);
        free(archive);
    }
}

inline int ArchiveLogInternal(const char* format, va_list args)
{
    // TODO Put that on stack?
    EnterCriticalSection(&archive->critical_section);
    memset(archive->buffer, 0, 0x10000);
    vsprintf_s(archive->buffer, 0x10000 - 1, format, args);
    if(!archive->is_init) {
        return ARCHIVE_ERROR_NOT_INITIALIZED;
    }
    if(archive->file_handle != NULL) {
        DWORD lpNumberOfBytesWritten;
        WriteFile(archive->file_handle,
                  &archive->buffer,
                  (DWORD)strlen(archive->buffer),
                  &lpNumberOfBytesWritten,
                  NULL);
    }
    if(archive->use_console)
        printf("%s", &archive->buffer);
    LeaveCriticalSection(&archive->critical_section);
}

int ArchiveLog(const char* format, ...)
{
    int status = 0;
    va_list args;

    va_start(args, format);
    status = ArchiveLogInternal(format, args);
    va_end(args);
    return status;
}

int ArchiveLogWithTs(const char* format, ...)
{
    int status = 0;
    va_list args;
    SYSTEMTIME t; 

    GetLocalTime(&t);
    status = ArchiveLog("[%.2d:%.2d:%.2d:%.4d] ",
                         t.wHour,
                         t.wMinute,
                         t.wSecond,
                         t.wMilliseconds);
    if(status != ARCHIVE_SUCCESS)
        return status;
    va_start(args, format);
    status = ArchiveLogInternal(format, args);
    va_end(args);
    return status;
}

int ArchiveHexDump(const uint8_t* data, size_t size)
{
    int status = ARCHIVE_SUCCESS;
    unsigned char *p = (unsigned char*)data;
    unsigned char c;
    size_t n;
    char bytestr[4] = {0};
    char addrstr[10] = {0};
    char hexstr[16 * 3 + 5] = {0};
    char charstr[16 * 1 + 5] = {0};

    for (n = 1; n <= size; n++) {
        if (n % 16 == 1) {
            sprintf_s(addrstr, sizeof(addrstr), "%.4x", (unsigned int)((ULONG_PTR)p - (ULONG_PTR)data));
        }
        c = *p;
        if (isprint(c) == 0) {
            c = '.';
        }
        sprintf_s(bytestr, sizeof(bytestr), "%02X ", *p);
        strncat_s(hexstr, sizeof(hexstr), bytestr, sizeof(hexstr) - strlen(hexstr) - 1);
        sprintf_s(bytestr, sizeof(bytestr), "%c", c);
        strncat_s(charstr, sizeof(charstr), bytestr, sizeof(charstr) - strlen(charstr) - 1);
        if (n % 16 == 0) {
            status = ArchiveLog("[%4.4s]   %-50.50s  %s\n", addrstr, hexstr, charstr);
            //printf("[%4.4s]   %-50.50s  %s\n", addrstr, hexstr, charstr);
            hexstr[0] = 0;
            charstr[0] = 0;
        }
        else if (n % 8 == 0) {
            strncat_s(hexstr, sizeof(hexstr), "  ", sizeof(hexstr)-strlen(hexstr)-1);
        }
        p++;
    }
    if (strlen(hexstr) > 0) {
        status = ArchiveLog("[%4.4s]   %-50.50s  %s\n", addrstr, hexstr, charstr);
        //printf("[%4.4s]   %-50.50s  %s\n", addrstr, hexstr, charstr);
    }
    return status;
}