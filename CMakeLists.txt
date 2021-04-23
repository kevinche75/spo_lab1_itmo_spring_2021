cmake_minimum_required(VERSION 3.19)
project(SPO_lab1 C)

set(CMAKE_C_STANDARD 99)
include_directories(${PROJECT_SOURCE_DIR})
add_executable(SPO_lab1 main.c available_devices.c ntfs.h ntfs.c ntfsutils.c ntfsutils.h)
add_library(ntfsutils SHARED ntfsutils.c ntfs.c available_devices.c)
target_link_libraries(SPO_lab1 PRIVATE ntfsutils blkid)