cmake_minimum_required(VERSION 3.12)
include(GenerateExportHeader) 

project(archive VERSION 0.1)

# Putting binaries in the output folder
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  ${PROJECT_BINARY_DIR}/bin)

# General poject settings
option(ARCHIVE_BUILD_SHARED_LIB "Build shared library" OFF)

if(ARCHIVE_BUILD_SHARED_LIB)
    add_library("archive" SHARED)
else()
    add_library("archive" STATIC)
endif()

# -----------------------------------------------------------------------------
# Generating configuration macros
# -----------------------------------------------------------------------------
generate_export_header("archive" BASE_NAME "ARCHIVE" 
                                 EXPORT_FILE_NAME "archive_export_config.h")
target_include_directories("archive"
    PUBLIC "include" ${PROJECT_BINARY_DIR})

target_sources("archive" PRIVATE "${CMAKE_CURRENT_LIST_DIR}/src/archive.c")
add_executable("test_archive" "${CMAKE_CURRENT_LIST_DIR}/tests/test_archive.c")
target_include_directories("test_archive" PRIVATE "${CMAKE_CURRENT_LIST_DIR}/include")
target_link_libraries("test_archive" PUBLIC "archive")