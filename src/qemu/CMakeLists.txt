set(target qemu)

function(create_qemu_group _srcs_var _path)
    set(_srcs ${${_srcs_var}})

    file(GLOB_RECURSE qemu_srcs ${_path}/*.c)
    file(GLOB_RECURSE qemu_hdrs include/${_path}/*.h ${_path}/*.h)
    file(GLOB_RECURSE shim_srcs pearpc-shim/${_path}/*.cc)
    file(GLOB_RECURSE shim_hdrs pearpc-shim/include/${_path}/*.h pearpc-shim/${_path}/*.h)

    source_group("${_path}"
        FILES
            ${qemu_srcs}
            ${qemu_hdrs}
    )

    source_group("${_path}\\shim"
        FILES
            ${shim_srcs}
            ${shim_hdrs}
    )

    # Force .c files to be compiled as c++
    SET_SOURCE_FILES_PROPERTIES(${qemu_srcs}
        PROPERTIES
            LANGUAGE CXX
    )

    list(APPEND _srcs ${qemu_srcs})
    list(APPEND _srcs ${qemu_hdrs})
    list(APPEND _srcs ${shim_srcs})
    list(APPEND _srcs ${shim_hdrs})

    set(${_srcs_var} ${_srcs} PARENT_SCOPE)
endfunction()

# Group together each QEMU system
create_qemu_group(qemu_srcs qemu)
create_qemu_group(qemu_srcs qom)
create_qemu_group(qemu_srcs hw)
create_qemu_group(qemu_srcs exec)
create_qemu_group(qemu_srcs sysemu)
create_qemu_group(qemu_srcs ui)

# Group together remaining shim sources
file(GLOB shim_srcs pearpc-shim/*.cc)
file(GLOB shim_hdrs pearpc-shim/include/*.h)

source_group("shim"
    FILES
        ${shim_srcs}
        ${shim_hdrs}
)

add_library(${target} ${qemu_srcs} ${shim_srcs} ${shim_hdrs})

target_compile_definitions(${target}
    PUBLIC
        PEARPC_SHIM
        $<$<STREQUAL:${OSAPI_DIR},posix>:CONFIG_POSIX>
        CONFIG_INT128
)

target_include_directories(${target}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/pearpc-shim
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/pearpc-shim/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
