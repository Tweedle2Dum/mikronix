cmake_minimum_required(VERSION 3.20)

project(mikronix LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(kernel
    kernel/src/main.cpp
)

target_include_directories(kernel PRIVATE
    ${CMAKE_SOURCE_DIR}/include/kernel
    ${CMAKE_SOURCE_DIR}/include/shared
)

target_compile_options(kernel PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -ffreestanding
    -mno-red-zone
    -fno-stack-protector
    -fno-stack-check
    -fno-lto
    -fno-PIC
    -ffunction-sections
    -fdata-sections
    -m64
    -march=x86-64
    -mabi=sysv
    -mno-80387
    -mno-mmx
    -mno-sse
    -mno-sse2
    -mcmodel=kernel
)

target_link_options(kernel PRIVATE
    "-T${CMAKE_SOURCE_DIR}/linkers/kernel.ld"
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
