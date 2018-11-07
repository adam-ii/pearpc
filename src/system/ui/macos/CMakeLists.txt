set(target system_ui)

set(all_srcs
    main.m
    gui.cc
    syskeyboard.cc
    sysmacos.h
    sysmacos.mm
    rendermetal.h
    rendermetal.mm
    shader_types.h
    AppDelegate.h
    AppDelegate.mm
    PearPCViewController.h
    PearPCViewController.mm
)

set(macosx_resources
    ${CMAKE_CURRENT_SOURCE_DIR}/en.lproj/PearPC.storyboard
)

set(metal_shaders
    ${CMAKE_CURRENT_SOURCE_DIR}/shaders.metal
)

set_source_files_properties(${metal_shaders}
    PROPERTIES
        LANGUAGE METAL
)

set(info_plist_template "${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in")

add_library(${target} ${all_srcs} ${info_plist_template} ${metal_shaders})

set_target_properties(${target}
    PROPERTIES
        MACOSX_INFO_PLIST "${info_plist_template}"
        MACOSX_RESOURCES "${macosx_resources}"
        METAL_SHADERS "${metal_shaders}"
)

set_property(TARGET ${target} APPEND_STRING PROPERTY COMPILE_FLAGS "-fobjc-arc")

find_library(PEARPC_COCOA_LIBRARY Cocoa)
find_library(PEARPC_METAL_LIBRARY Metal)
find_library(PEARPC_METALKIT_LIBRARY MetalKit)

target_compile_definitions(${target}
    PUBLIC
        PEARPC_UI_PROVIDES_MAIN
)

target_link_libraries(${target}
    PUBLIC
        ${PEARPC_COCOA_LIBRARY}
        ${PEARPC_METAL_LIBRARY}
        ${PEARPC_METALKIT_LIBRARY}
)
