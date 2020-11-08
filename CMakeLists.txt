cmake_minimum_required(VERSION 3.18)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 98)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_C_COMPILER x86_64-elf-gcc)
set(CMAKE_CXX_COMPILER x86_64-elf-gcc)
set(CMAKE_LD x86_64-elf-ld)
set(CMAKE_AR x86_64-elf-ar)

set(CMAKE_SYSTEM Generic)
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR x86_64)
set(CMAKE_CROSSCOMPILING 1)
set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_CXX_COMPILER_WORKS 1)

set(CMAKE_VERBOSE_MAKEFILE on)

set(COMPILER_FLAGS "-W -Wall -ansi -pedantic -std=c99 -O2 -ffreestanding -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs")
set(CMAKE_C_FLAGS ${COMPILER_FLAGS})
set(CMAKE_CXX_FLAGS ${COMPILER_FLAGS})

set(CMAKE_LINK_OBJECTS "<CMAKE_LD> --nmagic --script=${PROJECT_SOURCE_DIR}/linker.ld <OBJECTS> -o ${PROJECT_SOURCE_DIR}/iso/boot/kernel.bin <LINK_LIBRARIES>")
set(CMAKE_C_LINK_EXECUTABLE ${CMAKE_LINK_OBJECTS})
set(CMAKE_CXX_LINK_EXECUTABLE ${CMAKE_LINK_OBJECTS})
set(CMAKE_ASM_NASM_LINK_EXECUTABLE ${CMAKE_LINK_OBJECTS})

set(CMAKE_ASM_NASM_OBJECT_FORMAT elf64)

set(CMAKE_AR_OBJECTS "<CMAKE_AR> rcs <TARGET> <OBJECTS>")
set(CMAKE_C_ARCHIVE_CREATE ${CMAKE_AR_OBJECTS})
set(CMAKE_CXX_ARCHIVE_CREATE ${CMAKE_AR_OBJECTS})
set(CMAKE_ASM_NASM_ARCHIVE_CREATE ${CMAKE_AR_OBJECTS})


function(InstallVendor project_name git_repo)
        IF (NOT ${project_name})
                FetchContent_Declare(
                        ${project_name}
                        GIT_REPOSITORY ${git_repo}
                        SOURCE_DIR "${CMAKE_CURRENT_BINARY_DIR}/${project_name}-source"
                        BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/${project_name}"
                        SUBBUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/${project_name}-sub"
                )
                FetchContent_MakeAvailable(${project_name})
                set(${project_name} "")
                message(STATUS "Found ${project_name}")
        endif()
endfunction()

set(This CardinalOS)
set(VERSION 0.1)
project(${This} LANGUAGES C CXX ASM_NASM VERSION ${VERSION})

configure_file(${PROJECT_SOURCE_DIR}/include/Version.template ${PROJECT_SOURCE_DIR}/include/Version.h)

set(Headers
        ${PROJECT_SOURCE_DIR}/include/Version.h
        ${PROJECT_SOURCE_DIR}/src/core/boot.h
        ${PROJECT_SOURCE_DIR}/src/drivers/screen.h
        ${PROJECT_SOURCE_DIR}/src/include/stdint.h
        ${PROJECT_SOURCE_DIR}/src/kmain.h)

set(Sources
        ${PROJECT_SOURCE_DIR}/multiboot_header.asm
        ${PROJECT_SOURCE_DIR}/boot.asm
        ${PROJECT_SOURCE_DIR}/src/kmain.c
        ${PROJECT_SOURCE_DIR}/src/drivers/screen.c)

include_directories(${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR}/src/include)
add_library(kernel STATIC ${Sources})

# string(REPLACE " " ";" KERNEL_OBJECTS "")
# message(ERROR ${KERNEL_OBJECTS})

add_custom_command(TARGET kernel POST_BUILD
    COMMAND bash -c "${CMAKE_LD} --nmagic --output=${PROJECT_SOURCE_DIR}/iso/boot/kernel --script=${PROJECT_SOURCE_DIR}/linker.ld $(echo '$<TARGET_OBJECTS:kernel>' | sed 's/\;/ /g') $<TARGET_FILE:kernel>"
         VERBATIM
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    COMMENT "Linking..."
)

add_custom_command(TARGET kernel POST_BUILD
    COMMAND grub-mkrescue -o build/os.iso iso/
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    COMMENT "Building iso..."
)
