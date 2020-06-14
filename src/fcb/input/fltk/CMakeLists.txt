cmake_minimum_required (VERSION 3.10)

# FcbInput

file(GLOB_RECURSE HDRS *.h)
file(GLOB_RECURSE SRCS *.cpp)

add_library(FcbInput STATIC
    ${HDRS}
    ${SRCS}
)

target_include_directories(FcbInput
    PUBLIC
        api
    PRIVATE
        internal
)
target_include_directories(FcbInput SYSTEM PRIVATE
    ${FLTK_INCLUDE_DIR}
)

target_link_libraries(FcbInput PRIVATE
    ${FLTK_LIBRARIES}
)

target_compile_options(FcbInput PRIVATE ${FCB_WARNING_FLAGS} ${FCB_NO_FLTK_WARNING})

# set Visual Studio working directory
set_target_properties(FcbInput PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}")
