cmake_minimum_required(VERSION 3.18)

set(This CardinalOS)
set(VERSION 0.1)
set(KERNEL_NAME kernel)

# Compiler setup

set(CMAKE_C_COMPILER x86_64-elf-gcc)
set(CMAKE_CXX_COMPILER x86_64-elf-g++)
set(CMAKE_LD x86_64-elf-ld)
set(CMAKE_AR x86_64-elf-ar)

set(COMPILER_FLAGS "-W -Wall -std=c++17 -O2 -ffreestanding -nostdlib -nostdinc -fno-builtin -fstack-protector -nostartfiles -nodefaultlibs -fno-rtti -fno-threadsafe-statics -mno-red-zone -Wno-write-strings -Wno-pedantic -Wno-missing-field-initializers")
set(CMAKE_LINK_OBJECTS "<CMAKE_LD> --nmagic --script=${PROJECT_SOURCE_DIR}/linker.ld <OBJECTS> -o ${PROJECT_SOURCE_DIR}/iso/boot/kernel.bin <LINK_LIBRARIES>")
set(CMAKE_AR_OBJECTS "<CMAKE_AR> rcs <TARGET> <OBJECTS>")

set(CMAKE_C_FLAGS ${COMPILER_FLAGS})
set(CMAKE_C_ARCHIVE_CREATE ${CMAKE_AR_OBJECTS})
set(CMAKE_C_LINK_EXECUTABLE ${CMAKE_LINK_OBJECTS})

set(CMAKE_CXX_FLAGS ${COMPILER_FLAGS})
set(CMAKE_CXX_ARCHIVE_CREATE ${CMAKE_AR_OBJECTS})
set(CMAKE_CXX_LINK_EXECUTABLE ${CMAKE_LINK_OBJECTS})

set(CMAKE_ASM_NASM_OBJECT_FORMAT elf64)
set(CMAKE_ASM_NASM_ARCHIVE_CREATE ${CMAKE_AR_OBJECTS})
set(CMAKE_ASM_NASM_LINK_EXECUTABLE ${CMAKE_LINK_OBJECTS})

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

project(${This} LANGUAGES C CXX ASM_NASM VERSION ${VERSION})

configure_file(${PROJECT_SOURCE_DIR}/include/Version.template ${PROJECT_SOURCE_DIR}/include/Version.h)

# Sources

set(HEADERS
# etc
        ${PROJECT_SOURCE_DIR}/include/Version.h
# kernel
        ${PROJECT_SOURCE_DIR}/include/boot/multiboot.h
        ${PROJECT_SOURCE_DIR}/include/core/module.h
        ${PROJECT_SOURCE_DIR}/include/core/boot.h
# std
        ${PROJECT_SOURCE_DIR}/include/std/string.h
        ${PROJECT_SOURCE_DIR}/include/std/convert.h
        ${PROJECT_SOURCE_DIR}/include/std/io.h
        ${PROJECT_SOURCE_DIR}/include/std/memory.h)

set(SOURCES
# kernel
        ${PROJECT_SOURCE_DIR}/src/boot/multiboot.asm
        ${PROJECT_SOURCE_DIR}/src/boot/boot.asm
        ${PROJECT_SOURCE_DIR}/src/boot/long_mode.asm
        ${PROJECT_SOURCE_DIR}/src/core/boot.cpp
# std
        ${PROJECT_SOURCE_DIR}/src/std/string.cpp
        ${PROJECT_SOURCE_DIR}/src/std/convert.cpp
        ${PROJECT_SOURCE_DIR}/src/std/io.cpp
        ${PROJECT_SOURCE_DIR}/src/std/memory.cpp
        ${PROJECT_SOURCE_DIR}/src/std/stackguard.cpp
# c-entry
        ${PROJECT_SOURCE_DIR}/src/kmain.cpp)

set(KERNEL_OUTPUT ${PROJECT_SOURCE_DIR}/iso/boot/${KERNEL_NAME})

set(LINKER_FILE ${PROJECT_SOURCE_DIR}/src/linker/linker.ld)
add_library(${KERNEL_NAME} STATIC ${HEADERS} ${SOURCES})
target_include_directories(${KERNEL_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)

# Link static to binary elf64

add_custom_command(TARGET ${KERNEL_NAME} POST_BUILD
    COMMAND bash -c "${CMAKE_LD} --nmagic --output=${KERNEL_OUTPUT} --script=${LINKER_FILE} $(echo '$<TARGET_OBJECTS:${KERNEL_NAME}>' | sed 's/\;/ /g') $<TARGET_FILE:${KERNEL_NAME}>" VERBATIM
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    COMMENT "Linking..."
)

# Build ISO

add_custom_command(TARGET kernel POST_BUILD
    COMMAND grub-mkrescue -o build/os.iso iso/
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    COMMENT "Building iso..."
)
