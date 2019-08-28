
#include "archive.h"

int main(int argc, char** argv)
{
    ArchiveInit(0);
    ARCHIVE_LOG_TS("Test\n");
    ARCHIVE_LOG_TS("Test2\n");
    ARCHIVE_LOG("Test\n");
    ARCHIVE_LOG("Test2\n");
    return 0;
}