cmake_minimum_required (VERSION 3.10)

# FcbDatabase

file(GLOB_RECURSE HDRS *.h)
file(GLOB_RECURSE SRCS *.cpp)

add_library(FcbDatabase STATIC
    ${HDRS}
    ${SRCS}
)

target_include_directories(FcbDatabase
    PUBLIC
        api
    PRIVATE
        internal
)

target_link_libraries(FcbDatabase PRIVATE
    SqliteModernCpp
    FcbCore
)

target_compile_options(FcbDatabase PRIVATE ${FCB_WARNING_FLAGS})

# set Visual Studio working directory
set_target_properties(FcbDatabase PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}")
