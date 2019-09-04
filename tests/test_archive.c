
#include "archive.h"

int main(int argc, char** argv)
{
    ArchiveInit(0, NULL);
    ARCHIVE_LOG_TS("Test\n");
    ARCHIVE_LOG_TS("Test2\n");
    ARCHIVE_LOG("Test\n");
    ARCHIVE_LOG("Test2\n");
    ArchiveCleanup();
    ArchiveInit(0, "test.txt");
    ARCHIVE_LOG_TS("Test\n");
    ARCHIVE_LOG_TS("Test2\n");
    ARCHIVE_LOG("Test\n");
    ARCHIVE_LOG("Test2\n");
    ArchiveCleanup();
    return 0;
}