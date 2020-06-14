cmake_minimum_required (VERSION 3.10)

# Util

add_library(Util INTERFACE)

# Add Eigen's Natvis (debugger pretty-printer for Visual Studio) to source files
if (MSVC)
    target_sources(Util INTERFACE ${EIGEN_SRC}/debug/msvc/eigen.natvis)
endif (MSVC)

target_include_directories(Util INTERFACE
    api
)
target_include_directories(Util SYSTEM INTERFACE
    ${EIGEN_SRC}
)

# Add a project for IDE convenience
file(GLOB_RECURSE HDRS *.h)
add_custom_target(Util_ SOURCES
    ${HDRS}
)
