cmake_minimum_required (VERSION 3.10)

# GuiFltk

file(GLOB_RECURSE HDRS *.h)
file(GLOB_RECURSE SRCS *.cpp)

add_library(GuiFltk STATIC
    ${HDRS}
    ${SRCS}
)

target_include_directories(GuiFltk
    PUBLIC
        api
    PRIVATE
        internal
)
target_include_directories(GuiFltk SYSTEM PRIVATE
    ${FLTK_INCLUDE_DIR}
)

target_link_libraries(GuiFltk PRIVATE
    ${FLTK_LIBRARIES}
    ${OPENGL_LIBRARIES}
    FcbGraphics
    FcbInput
)

target_compile_options(GuiFltk PRIVATE ${FCB_WARNING_FLAGS} ${FCB_NO_FLTK_WARNING})

# set Visual Studio working directory
set_target_properties(GuiFltk PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}")
