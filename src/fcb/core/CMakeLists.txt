cmake_minimum_required (VERSION 3.10)

# FcbCore

file(GLOB_RECURSE HDRS *.h)
file(GLOB_RECURSE SRCS *.cpp)

add_library(FcbCore STATIC
    ${HDRS}
    ${SRCS}
)

target_include_directories(FcbCore
    PUBLIC
        api
    PRIVATE
        internal
)

target_link_libraries(FcbCore PUBLIC
    ML
    Util
)

target_compile_options(FcbCore PRIVATE ${FCB_WARNING_FLAGS})

# set Visual Studio working directory
set_target_properties(FcbCore PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}")
