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
    FcbCore
    FcbGraphics
    FcbInput
    FcbDatabase
    GuiFltk
)

target_compile_options(FcbExec PRIVATE ${FCB_WARNING_FLAGS})

# set Visual Studio working directory
set_target_properties(FcbExec PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}")

if(USE_INTERNAL_SQLITE3)
    # Copy DLL (if different) to the executable's directory
    add_custom_command(TARGET FcbExec POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${CMAKE_SOURCE_DIR}/third-party/sqlite3/lib/windows/sqlite3.dll
                ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}
    )
endif()
