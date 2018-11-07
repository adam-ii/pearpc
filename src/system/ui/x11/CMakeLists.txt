set(target system_ui)

include(CMakeFindDependencyMacro)
find_dependency(X11 REQUIRED)

set(all_srcs
    gui.cc
    sysdisplay.cc
    syskeyboard.cc
    sysmouse.cc
    sysx11.cc
    sysx11.h
    x_keymap.c
    x_keymap.h
    input-keymap-xorgevdev-to-qcode.c
    input-keymap-xorgkbd-to-qcode.c
    input-keymap-xorgxquartz-to-qcode.c
    input-keymap-xorgxwin-to-qcode.c
)

include(${CMAKE_CURRENT_SOURCE_DIR}/FindGLIB.cmake)

add_library(${target} ${all_srcs})

target_include_directories(${target}
    PUBLIC
        ${X11_X11_INCLUDE_PATH}
    PRIVATE
        ${GLIB_INCLUDE_DIR}
        ${GLIBCONFIG_INCLUDE_DIR}
)

target_link_libraries(${target}
    PUBLIC
        ${X11_X11_LIB}
        ${GLIB_LIBRARIES}
)
