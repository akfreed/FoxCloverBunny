cmake_minimum_required (VERSION 3.10)

# C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Solution
project(FoxCloverBunny CXX)

include(CheckCXXCompilerFlag)

# Architecture-dependent settings
if(MSVC)
    # Warning flags
    set(FCB_WARNING_FLAGS /W4 /WX)
    # Enable AVX (Vectorized Instructions)
    CHECK_CXX_COMPILER_FLAG("/arch:AVX2" COMPILER_OPT_ARCH_AVX_SUPPORTED)
    if (COMPILER_OPT_ARCH_AVX_SUPPORTED)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:AVX2")
    endif (COMPILER_OPT_ARCH_AVX_SUPPORTED)
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    # Warning flags
    set(FCB_WARNING_FLAGS -Wall -Wextra -pedantic -Wconversion -Werror)
    # Enable AVX (Vectorized Instructions)
    CHECK_CXX_COMPILER_FLAG("-march=native" COMPILER_OPT_ARCH_NATIVE_SUPPORTED)
    if(COMPILER_OPT_ARCH_NATIVE_SUPPORTED)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
    endif(COMPILER_OPT_ARCH_NATIVE_SUPPORTED)
endif()

# -------------------------------------------------------------------
# External dependencies

find_package(Threads REQUIRED)

# On Ubuntu 18, install libgl1-mesa-dev.
find_package(OpenGL REQUIRED)

# Eigen
set(EIGEN_SRC ${CMAKE_SOURCE_DIR}/third-party/eigen)
# Exclude LGPL code
add_definitions(-DEIGEN_MPL2_ONLY)

# Add FLTK
set(FLTK_SKIP_FLUID TRUE)
find_package(FLTK)
if(NOT FLTK_FOUND)
    if(NOT MSVC)
        # On Ubuntu 18, install libfltk1.3-dev.
        message(FATAL_ERROR "Could not find FLTK. Install libfltk1.3-dev (Ubuntu).")
    else()
        # See the README if these dont't build.
        message("Trying pre-built binaries (MSVC x64).")
        include(${CMAKE_SOURCE_DIR}/third-party/fltk/windows-msvc/CMake/FLTKConfig.cmake)
        set(FLTK_INCLUDE_DIR ${FLTK_INCLUDE_DIRS})
        set(FLTK_LIBRARIES fltk_z fltk_jpeg fltk_png fltk fltk_forms fltk_images fltk_gl)
    endif()
endif()
foreach(path ${FLTK_INCLUDE_DIR})
    if(NOT EXISTS ${path})
        message(FATAL_ERROR "find_package set bad value for FLTK include directory. Directory does not exist: " ${path})
    endif()
endforeach()
# FLTK has warnings
if(MSVC)
    set(FCB_NO_FLTK_WARNING
        /experimental:external
        /external:I ${FLTK_INCLUDE_DIR}
        /external:W0
    )
endif()

# Add PerformanceTimer
add_subdirectory(third-party/PerformanceTimer/PerformanceTimer98)

# Add SQLite
include(cmake/find-sqlite3.cmake)

# Add SqliteModernCpp
add_library(SqliteModernCpp INTERFACE)
target_include_directories(SqliteModernCpp SYSTEM INTERFACE
    ${CMAKE_SOURCE_DIR}/third-party/sqlite_modern_cpp/hdr
)
target_link_libraries(SqliteModernCpp INTERFACE
    Sqlite3
)
if(MSVC)
    target_compile_options(SqliteModernCpp INTERFACE
        /experimental:external
        /external:I ${CMAKE_SOURCE_DIR}/third-party/sqlite_modern_cpp/hdr
        /external:W0)
endif()


# -------------------------------------------------------------------
# Projects

add_subdirectory(src/util)
add_subdirectory(src/ml)
add_subdirectory(src/fcb)
add_subdirectory(src/gui/fltk)

# -------------------------------------------------------------------

# Set FcbExec as the start-up project
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT FcbExec)
