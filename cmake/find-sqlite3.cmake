# Fails if sqlite3 is not found.
#
# Sets these variables:
#     SQLite3_INCLUDE_DIRS
#     SQLite3_LIBRARIES
#     USE_INTERNAL_SQLITE3 - True if MSVC and using the provided sqlite3.dll.
#
# Creates this target:
#     Sqlite3

find_path(SQLite3_INCLUDE_DIRS sqlite3.h)
find_library(SQLite3_LIBRARIES sqlite3)
if(MSVC AND (NOT SQLite3_INCLUDE_DIRS OR NOT SQLite3_LIBRARIES))
    find_path(SQLite3_INCLUDE_DIRS sqlite3.h
        PATHS ${CMAKE_SOURCE_DIR}/third-party/sqlite3/include
        NO_DEFAULT_PATH)
    find_library(SQLite3_LIBRARIES sqlite3
        PATHS ${CMAKE_SOURCE_DIR}/third-party/sqlite3/lib/windows
        NO_DEFAULT_PATH)
    set (USE_INTERNAL_SQLITE3 TRUE CACHE BOOL "Copy the provided sqlite3.dll (for MSVC only).")
endif()
if(NOT SQLite3_INCLUDE_DIRS OR NOT SQLite3_LIBRARIES)
    message("Unable to locate SQLite3.")
    message("    SQLite3_INCLUDE_DIRS: " ${SQLite3_INCLUDE_DIRS})
    message("    SQLite3_LIBRARIES:    " ${SQLite3_LIBRARIES})
    if(NOT MSVC)
        message("Install sqlite3 and libsqlite3-dev (Ubuntu).")
    endif()
    message(FATAL_ERROR "")
endif()

add_library(Sqlite3 INTERFACE)
target_include_directories(Sqlite3 SYSTEM INTERFACE
    ${SQLite3_INCLUDE_DIRS}
)
target_link_libraries(Sqlite3 INTERFACE
    ${SQLite3_LIBRARIES}
)
