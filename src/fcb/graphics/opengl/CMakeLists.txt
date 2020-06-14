cmake_minimum_required (VERSION 3.10)

# FcbGraphics

file(GLOB_RECURSE HDRS *.h)
file(GLOB_RECURSE SRCS *.cpp)

add_library(FcbGraphics STATIC
    ${HDRS}
    ${SRCS}
)

target_include_directories(FcbGraphics
    PUBLIC
        api
    PRIVATE
        internal
)
target_include_directories(FcbGraphics SYSTEM PRIVATE
    ${FLTK_INCLUDE_DIR}
)

target_link_libraries(FcbGraphics PRIVATE
    ${FLTK_LIBRARIES}
    ${OPENGL_LIBRARIES}
    FcbCore
)

target_compile_options(FcbGraphics PRIVATE ${FCB_WARNING_FLAGS} ${FCB_NO_FLTK_WARNING})

# set Visual Studio working directory
set_target_properties(FcbGraphics PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}")
