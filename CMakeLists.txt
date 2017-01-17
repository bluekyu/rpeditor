# CMakeList.txt template for executable.
# Author: Younguk Kim (bluekyu)
# Date  : 2016-05-18

cmake_minimum_required(VERSION 3.5)
project(rpeditor)

# === configure ====================================================================================
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)    # Project Grouping

# check package
include(FindPackages)

# debug
#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D")
set(CMAKE_DEBUG_POSTFIX "d")

# platform specific configure
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -flto")

    # build type
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE "Release" CACHE STRING
            "Choose the type of build, options are: None Debug Release."
            FORCE)
    endif()
    message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    if(MSVC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
        set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /LTCG")
        set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /LTCG")
    endif()
else()
    message(WARNING "Unsupported system: ${CMAKE_SYSTEM_NAME}")
endif()

# subdirectory
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src")      # /src
include_directories("${PROJECT_BINARY_DIR}")                # /build

# set input files
include("${PROJECT_SOURCE_DIR}/files.cmake")
# ==================================================================================================

# === build & install ==============================================================================
# create executable
add_executable(${PROJECT_NAME} WIN32 ${rpeditor_sources})
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES} ${Qt_LIBRARIES})

# install
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION "bin"
    LIBRARY DESTINATION "lib"
    ARCHIVE DESTINATION "lib")
