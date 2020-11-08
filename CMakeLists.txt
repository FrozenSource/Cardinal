cmake_minimum_required(VERSION 3.18)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(HUNTER_PACKAGES GTest)
include(FetchContent)
FetchContent_Declare(SetupHunter GIT_REPOSITORY https://github.com/cpp-pm/gate)
FetchContent_MakeAvailable(SetupHunter)

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
                set(${project_name} CACHE INTERNAL "")
                message(STATUS "Found ${project_name}")
        endif()
endfunction()

set(This project)
set(VERSION 0.1)
project(${This} LANGUAGES C CXX VERSION ${VERSION})

configure_file(${PROJECT_SOURCE_DIR}/include/Version.template ${PROJECT_SOURCE_DIR}/include/Version.h)

find_package(GTest CONFIG REQUIRED)

InstallVendor(Core https://CorrupD:hiddenhand@git.nipqer.nl/CorrupD/Core.git)

set(Headers
        ${PROJECT_SOURCE_DIR}/include/Version.h)

set(Sources
	"")

set(Test-Sources
	"")

include_directories(${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/test)
add_library(${This}-lib ${Headers} ${Sources})
add_executable(${This} ${PROJECT_SOURCE_DIR}/src/main.cpp)
add_executable(${This}-test ${Test-Sources} ${PROJECT_SOURCE_DIR}/test/main-test.cpp)

target_link_libraries(${This}-lib PUBLIC Core)
target_link_libraries(${This} PUBLIC ${This}-lib Core)
target_link_libraries(${This}-test PUBLIC GTest::gtest ${This}-lib Core)

if(WIN32)
        target_compile_definitions(${This}-lib PUBLIC BUILD _CRT_SECURE_NO_WARNINGS)
endif()

enable_testing()

add_test(
        NAME ${This}-test
        COMMAND ${This}-test
)