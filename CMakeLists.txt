cmake_minimum_required(VERSION 3.16.3)

if (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/sdkconfig)
    message(FATAL_ERROR "sdkconfig not found (please run ./switchconf.sh)")
endif()

if (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/config.cmake)
    message(FATAL_ERROR "config.cmake not found (please run ./switchconf.sh)")
endif()

file(READ_SYMLINK ${CMAKE_CURRENT_SOURCE_DIR}/sdkconfig result)
if (result STREQUAL "")
    message(FATAL_ERROR "sdkconfig is not a symlink (please run ./switchconf.sh)")
endif()

file(READ_SYMLINK ${CMAKE_CURRENT_SOURCE_DIR}/config.cmake result)
if (result STREQUAL "")
    message(FATAL_ERROR "config.cmake is not a symlink (please run ./switchconf.sh)")
endif()

include(config.cmake)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-volatile")

idf_build_set_property(CXX_COMPILE_OPTIONS -std=gnu++23 APPEND)

set(EXTRA_COMPONENT_DIRS
    esp-protocols/components/mdns
    esp-protocols/components/esp_websocket_client
    esp-protocols/components/asio
)

set(GIT_MATCH "v[0-9]+\\.[0-9]+\\.[0-9]+")

set(_repo_path "${CMAKE_CURRENT_LIST_DIR}")
git_describe(GIT_DESCRIBE "${_repo_path}" "--match=${GIT_MATCH}*")

message(STATUS "GIT_DESCRIBE=${GIT_DESCRIBE}")

if(GIT_DESCRIBE)
    STRING(REGEX REPLACE "^${GIT_MATCH}" "" PROJECT_VER ${GIT_DESCRIBE})
else()
    message(FATAL_ERROR "Not a git repository, using 1.0.0 as version")
    set(PROJECT_VER 1)
endif()

message(STATUS "PROJECT_VER=${PROJECT_VER}")

project(${BOBBY_APP_NAME})

set(expected_build_folder "${CMAKE_CURRENT_SOURCE_DIR}/build")
file(READ_SYMLINK "${expected_build_folder}" expected_build_folder_resolved)
if(NOT IS_ABSOLUTE "${expected_build_folder_resolved}")
  get_filename_component(dir "${expected_build_folder}" DIRECTORY)
  set(expected_build_folder_resolved "${dir}/${expected_build_folder_resolved}")
endif()

if (NOT PROJECT_BINARY_DIR STREQUAL "${expected_build_folder}" AND NOT PROJECT_BINARY_DIR STREQUAL "${expected_build_folder_resolved}")
    message(FATAL_ERROR "building in ${PROJECT_BINARY_DIR} is not supported, you have to build in ${expected_build_folder} or ${expected_build_folder_resolved} (please run ./switchconf.sh)")
endif()
