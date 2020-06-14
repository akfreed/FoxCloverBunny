cmake_minimum_required (VERSION 3.10)

# FcbExec

file(GLOB_RECURSE HDRS *.h)
file(GLOB_RECURSE SRCS *.cpp)

add_executable(FcbExec
    ${HDRS}
    ${SRCS}
)

target_include_directories(FcbExec PRIVATE
    .
)

target_link_libraries(FcbExec PRIVATE
    ${CMAKE_THREAD_LIBS_INIT}
    PerformanceTimer98
    Util
    FcbCore
    FcbGraphics
    FcbInput
    GuiFltk
)

target_compile_options(FcbExec PRIVATE ${FCB_WARNING_FLAGS})

# set Visual Studio working directory
set_target_properties(FcbExec PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}")
