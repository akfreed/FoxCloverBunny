cmake_minimum_required (VERSION 3.10)

# ML

file(GLOB_RECURSE HDRS *.h)
file(GLOB_RECURSE SRCS *.cpp)

add_library(ML STATIC
    ${HDRS}
    ${SRCS}
)
# Add Eigen's Natvis (debugger pretty-printer for Visual Studio) to source files
if (MSVC)
    target_sources(ML PUBLIC ${EIGEN_SRC}/debug/msvc/eigen.natvis)
endif (MSVC)

target_include_directories(ML
    PUBLIC
        api
    PRIVATE
        internal
)
target_include_directories(ML SYSTEM PUBLIC
    ${EIGEN_SRC}
)

target_link_libraries(ML PRIVATE
    Util
)

target_compile_options(ML PRIVATE ${FCB_WARNING_FLAGS})

# set Visual Studio working directory
set_target_properties(ML PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}")
