set(target cpu_common)

set(all_srcs
    common.h
    cpu.cc
    cpu.h
    debug.h
    mem.h
)

add_library(${target} ${all_srcs})

if (${PEARPC_CPU} STREQUAL "generic")
    add_subdirectory(cpu_generic)
elseif (${PEARPC_CPU} STREQUAL "x86")
    add_subdirectory(cpu_jitc_x86)
elseif (${PEARPC_CPU} STREQUAL "x86_64")
    add_subdirectory(cpu_jitc_x86_64)
endif()
