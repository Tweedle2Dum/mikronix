cmake_minimum_required(VERSION 3.21)

project(mikronix LANGUAGES C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(KERNEL_ARCH_FLAGS
    $<$<NOT:$<BOOL:${CMAKE_BUILD_TYPE}>>:-O2>
    $<$<NOT:$<BOOL:${CMAKE_BUILD_TYPE}>>:-g>
    -ffreestanding
    -fno-builtin
    -fno-strict-aliasing
    -mno-red-zone
    -fno-stack-protector
    -fno-stack-check
    -fno-lto
    -fno-PIC
    -fno-pie
    -fno-asynchronous-unwind-tables
    -fno-unwind-tables
    -ffunction-sections
    -fdata-sections
    -m64
    -march=x86-64
    -mabi=sysv
    -mno-80387
    -mno-mmx
    -mno-sse
    -mno-sse2
    -msoft-float
    -mcmodel=kernel
)

# ---------------------------------------------------------------------------
# External dependencies
# ---------------------------------------------------------------------------

add_library(flanterm OBJECT
    vendor/flanterm/src/flanterm.c
    vendor/flanterm/src/flanterm_backends/fb.c
)

target_include_directories(flanterm SYSTEM PUBLIC
    ${CMAKE_SOURCE_DIR}/vendor/flanterm/src
)

target_compile_options(flanterm PRIVATE
    ${KERNEL_ARCH_FLAGS}
)

# ---------------------------------------------------------------------------
# Kernel
# ---------------------------------------------------------------------------

add_executable(kernel
    kernel/src/main.c
    kernel/src/memory.c
)

target_include_directories(kernel PRIVATE
    ${CMAKE_SOURCE_DIR}/include/kernel
    ${CMAKE_SOURCE_DIR}/include/shared
)

target_compile_options(kernel PRIVATE
    ${KERNEL_ARCH_FLAGS}
    -Wall
    -Wextra
    -Wpedantic
    -Wstrict-prototypes
    -Werror=implicit-function-declaration
)

target_link_libraries(kernel PRIVATE flanterm)

target_link_options(kernel PRIVATE
    "-T${CMAKE_SOURCE_DIR}/linkers/kernel.ld"
    -static
    --gc-sections
    "SHELL:-z max-page-size=0x1000"
    "SHELL:-z noexecstack"
)

# ---------------------------------------------------------------------------
# Limine ISO
# ---------------------------------------------------------------------------

set(LIMINE_DIR "${CMAKE_SOURCE_DIR}/boot/limine")
set(ISO_DIR "${CMAKE_BINARY_DIR}/iso")
set(ISO_IMAGE "${CMAKE_BINARY_DIR}/mikronix.iso")

add_custom_target(iso
    COMMAND ${CMAKE_COMMAND} -E rm -rf "${ISO_DIR}"

    COMMAND ${CMAKE_COMMAND} -E make_directory
        "${ISO_DIR}/boot"
    COMMAND ${CMAKE_COMMAND} -E make_directory
        "${ISO_DIR}/EFI/BOOT"

    COMMAND ${CMAKE_COMMAND} -E copy
        "$<TARGET_FILE:kernel>"
        "${ISO_DIR}/boot/kernel"

    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_SOURCE_DIR}/config/limine.conf"
        "${ISO_DIR}/limine.conf"

    COMMAND ${CMAKE_COMMAND} -E copy
        "${LIMINE_DIR}/limine-bios.sys"
        "${ISO_DIR}/boot/limine-bios.sys"

    COMMAND ${CMAKE_COMMAND} -E copy
        "${LIMINE_DIR}/limine-bios-cd.bin"
        "${ISO_DIR}/boot/limine-bios-cd.bin"

    COMMAND ${CMAKE_COMMAND} -E copy
        "${LIMINE_DIR}/limine-uefi-cd.bin"
        "${ISO_DIR}/boot/limine-uefi-cd.bin"

    COMMAND ${CMAKE_COMMAND} -E copy
        "${LIMINE_DIR}/BOOTX64.EFI"
        "${ISO_DIR}/EFI/BOOT/BOOTX64.EFI"

    COMMAND xorriso -as mkisofs
        -R -r -J
        -b boot/limine-bios-cd.bin
        -no-emul-boot
        -boot-load-size 4
        -boot-info-table
        -hfsplus
        -apm-block-size 2048
        --efi-boot boot/limine-uefi-cd.bin
        -efi-boot-part
        --efi-boot-image
        --protective-msdos-label
        "${ISO_DIR}"
        -o "${ISO_IMAGE}"

    COMMAND "${LIMINE_DIR}/limine"
        bios-install "${ISO_IMAGE}"

    DEPENDS kernel
    VERBATIM
)

add_custom_target(iso-clean
    COMMAND ${CMAKE_COMMAND} -E rm -rf
        "${ISO_DIR}"
        "${ISO_IMAGE}"
)


find_program(CLANG_FORMAT NAMES clang-format)
if(CLANG_FORMAT)
    # Gather all C source and header files recursively
    file(GLOB_RECURSE FORMAT_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/kernel/src/*.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/kernel/include/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/**/*.h"
    )

    add_custom_target(format
        COMMAND ${CLANG_FORMAT} -i ${FORMAT_SOURCES}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Formatting C source and header files with clang-format..."
    )
else()
    message(STATUS "clang-format not found;")
endif()
