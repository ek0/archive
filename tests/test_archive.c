
#include "archive.h"

#include <string.h>
#include <stdlib.h>

const char data[] = "asdkjfhasjkfhasjklfhaskldfha;sqwioeruqwpoiruiofjsdklhas";

int main(int argc, char** argv)
{
    char* temp_buff = malloc(0x5000);
    if(temp_buff == NULL)
        return;
    memset(temp_buff, 0, 0x5000);
    ArchiveInit(0, NULL);
    ARCHIVE_LOG_TS("Test\n");
    ARCHIVE_LOG_TS("Test2\n");
    ARCHIVE_LOG("Test\n");
    ArchiveHexDump(data, sizeof(data));
    ARCHIVE_LOG("Test2\n");
    ArchiveHexDump(temp_buff, 0x5000);
    ArchiveCleanup();
    ArchiveInit(0, "test.txt");
    ARCHIVE_LOG_TS("Test\n");
    ARCHIVE_LOG_TS("Test2\n");
    ARCHIVE_LOG("Test\n");
    ARCHIVE_LOG("Test2\n");
    ArchiveCleanup();
    free(temp_buff);
    return 0;
}