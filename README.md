# Archive
Archive is a small logging utility that can be used easily from any project.

# API Documentation

## ArchiveInit
```c
void ArchiveInit(int options, const char* path);
```
Initialize the archive handler.

### Parameters
#### `options`
Can be one of the following values:

- `USE_CONSOLE` Pop up a new console
- `NO_LOG_FILE` Does not pop a log file

#### `path`
Name of the log file. If `path` is NULL the log file will be named using the following format: `DD-MM-YYYY_HH-MN-SEC.txt` where:
```
YYYY = Current Year at initialization
MM = Curent Month at initialization
DD = Current day at initialization
HH = Hour at initialization
MN = Minutes at initialization
SEC = Seconds at initialization
```
### Return Value
`ARCHIVE_SUCCESS` on success.

`ARCHIVE_ERROR_NOT_INITITALIZED` if an error happened during the memory allocation.

`ARCHIVE_ERROR_INVALID_FILE_HANDLE` if an error occured during the file creation.
## ArchiveCleanup
```c
void ArchiveCleanup();
```
Deallocate all resources associated with Archive.

## ArchiveLog
```c
void ArchiveLog(const char* format, ...[args]);
void ArchiveLogWithTs(const char* format, ...[args]);
void ARCHIVE_LOG(const char* format, ...[args]);
void ARCHIVE_LOG_TS(const char* format, ...[args]);
```
Logging utilities. To be called after `ArchiveInit`.
### Parameters
#### `format`
Format string.
#### `args`
Optional arguments.
### Return Value
`ARCHIVE_SUCCESS` on success.

`ARCHIVE_ERROR_NOT_INITIALIZED` if Archive has not been initialized through `ArchiveInit`
## ArchiveHexDump
```c
void ArchiveHexDump(const uint8_t* data, size_t size);
```
Utility function used to display binary data into an hexadecimal. To be called after `ArchiveInit`.
### Parameters
#### `data`
Data to be displayed
#### `size`
Size of the data to be displayed
### Return Value
`ARCHIVE_SUCCESS` on success.

`ARCHIVE_ERROR_NOT_INITIALIZED` if Archive has not been properly initialized through `ArchiveInit`

# Examples
Archive can be use as the following:
```c
int main(int argc, char** argv)
{
    ArchiveInit(0, NULL);            // Will create 03-09-2019_19-53-23.txt for example
    ARCHIVE_LOG("My Awesome Log\n"); // Will output "main: My Awesome Log"
    ArchiveLog("Second line\n");     // Will output "Second line"
    ARCHVIE_LOG_TS("Third line\n");  // Will output "[19:53:24:0782] main: Third line"
    ArchiveLogWithTs("Fourth\n");    // Will output "[19:53:24:0971] Fourth"
    ArchiveCleanup();
    return 0;
}
```

Instead, you can also specify a filename, if the file already exists it will be overwritten:
```c
int main(int argc, char** argv)
{
    ArchiveInit(0, "test.txt"); // Will only create test.txt in the current directory
    ARCHIVE_LOG("My Awesome Log\n");
    ArchiveCleanup();
    return 0;
}
```
Additionally, you can spawn a console by specifying the options to the initialization:
```c
int main(int argc, char** argv)
{
    ArchiveInit(USE_CONSOLE, NULL); // This will open a new terminal and outputs the log here as well
    ARCHIVE_LOG("My Awesome Log\n");
    ArchiveCleanup();
    return 0;
}
```