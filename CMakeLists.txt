cmake_minimum_required(VERSION 3.19)
project(SPO_lab1 C)

set(CMAKE_C_STANDARD 99)
include_directories(${PROJECT_SOURCE_DIR})
add_executable(SPO_lab1 main.c available_devices.c check_ntfs.c check_ntfs.h ntfs.h ntfs.c)
target_link_libraries(SPO_lab1 blkid)