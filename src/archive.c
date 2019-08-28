
#include "mlist.h"

#include <Windows.h>
#include <stdio.h>
#include <stdint.h>

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
    char buffer[0x10000];
    HANDLE file_handle;
    LIST_ENTRY log_files;
};

static struct ArchiveHandle *archive = NULL;

void ArchiveSetupLoggingDirectory()
{
    if(archive == NULL)
        return;
    SYSTEMTIME local_time;
    GetLocalTime(&local_time);
    GetCurrentDirectory(MAX_PATH - 1, archive->current_directory);
    if (archive->is_critical_section_init == 0) {
        InitializeCriticalSection(&archive->critical_section);
        archive->is_critical_section_init = 1;
    }
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
    //CreateDirectory(archive->log_directory, NULL);
    // TODO: Temporary
    archive->file_handle = CreateFile(archive->log_directory,
                                      GENERIC_READ | GENERIC_WRITE,
                                      FILE_SHARE_READ | FILE_SHARE_WRITE,
                                      NULL,
                                      CREATE_ALWAYS,
                                      FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH,
                                      NULL);
    archive->is_init = 1;
}

void ArchiveInit(int use_console)
{
    archive = malloc(sizeof(struct ArchiveHandle));
    memset(archive, 0, sizeof(sizeof(struct ArchiveHandle)));
    //InitializeListHead(&archive->log_files);
    ArchiveSetupLoggingDirectory();
    archive->use_console = use_console;
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
        CloseHandle(archive->file_handle);
        free(archive);
    }
}

VOID ArchiveLogToFile(void *data, size_t size)
{
    static int is_init = 0;
    FILE *fFpbin = NULL;
    static HANDLE file_handle = INVALID_HANDLE_VALUE;

    EnterCriticalSection(&archive->critical_section);
    if(archive->file_handle != NULL) {
        DWORD lpNumberOfBytesWritten;
        WriteFile(archive->file_handle, data, (DWORD)size, &lpNumberOfBytesWritten, NULL);
    }
    LeaveCriticalSection(&archive->critical_section);
}

struct LogFileInfo* ArchiveGetFileInfoFromThreadId(uint32_t thread_id)
{
    LIST_ENTRY *entry = NULL;

    while(entry != &archive->log_files)
    {
        entry = RemoveHeadList(&archive->log_files);
    }
    return NULL;
}

struct LogFileInfo* ArchiveCreateLogFile(uint32_t thread_id)
{
    struct LogFileInfo *file_info = NULL;
    
    file_info = malloc(sizeof(struct LogFileInfo));
    sprintf_s(file_info->file_path,
              MAX_PATH,
              "%s\\%d.txt",
              &archive->log_directory,
              thread_id);
    file_info->file_handle = CreateFile(file_info->file_path,
                                        GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
                                        NULL,
                                        CREATE_ALWAYS,
                                        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH,
                                        NULL);
    InsertHeadList(&archive->log_files, &file_info->next);
    return NULL;
}

inline void ArchiveLogInternal(const char* format, va_list args)
{
    //struct LogFileInfo *log_file = NULL;
    //uint32_t current_tid = 0;
    //va_list args;

    EnterCriticalSection(&archive->critical_section);
    //va_start(args, format);
    memset(archive->buffer, 0, 0x10000);
    vsprintf_s(archive->buffer, 0x10000 - 1, format, args);
    //va_end(args);
    if(!archive->is_init) {
        return;
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

void ArchiveLog(const char* format, ...)
{
    va_list args;

    va_start(args, format);
    ArchiveLogInternal(format, args);
    va_end(args);
}

void ArchiveLogWithTs(const char* format, ...)
{
    va_list args;
    SYSTEMTIME t; 

    GetLocalTime(&t);
    ArchiveLog("[%.2d:%.2d:%.2d:%.4d] ",
               t.wHour,
               t.wMinute,
               t.wSecond,
               t.wMilliseconds);
    va_start(args, format);
    ArchiveLogInternal(format, args);
    va_end(args);
}

void ArchiveHexDump(const uint8_t* data, size_t size)
{
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
            ArchiveLog("[%4.4s]   %-50.50s  %s\n", addrstr, hexstr, charstr);
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
        ArchiveLog("[%4.4s]   %-50.50s  %s\n", addrstr, hexstr, charstr);
        //printf("[%4.4s]   %-50.50s  %s\n", addrstr, hexstr, charstr);
    }
    //LeaveCriticalSection(&CriticalSection);
}