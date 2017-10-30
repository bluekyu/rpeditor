# Author: Younguk Kim (bluekyu)
# Date  : 2016-05-18

cmake_minimum_required(VERSION 3.8)
project(rpeditor)

# === configure ====================================================================================
set_property(GLOBAL PROPERTY USE_FOLDERS ON)    # Project Grouping

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# check package
include(FindPackages)
if(WIN32)
    include(windows_utils)
endif()

# === project specific packages ===
# find spdlog
if(NOT TARGET spdlog::spdlog)
    find_package(spdlog REQUIRED)
endif()

# set input files
include("${PROJECT_SOURCE_DIR}/files.cmake")
# ==================================================================================================

# === target =======================================================================================
add_executable(${PROJECT_NAME} WIN32 ${rpeditor_sources})

target_compile_features(${PROJECT_NAME} PRIVATE cxx_auto_type)
if(MSVC)
    windows_add_longpath_manifest(${PROJECT_NAME})
    target_compile_features(${PROJECT_NAME} PUBLIC cxx_final cxx_strong_enums cxx_delegating_constructors
        PRIVATE $<$<VERSION_GREATER:${MSVC_VERSION},1800>:cxx_generic_lambdas>
    )
    target_compile_options(${PROJECT_NAME} PRIVATE /MP /wd4251 /utf-8 /std:c++17)
    set_property(TARGET ${PROJECT_NAME} APPEND_STRING PROPERTY LINK_FLAGS_RELWITHDEBINFO    " /INCREMENTAL:NO /OPT:REF /OPT:ICF ")
    set_property(TARGET ${PROJECT_NAME} APPEND_STRING PROPERTY LINK_FLAGS_RELEASE           " /INCREMENTAL:NO /OPT:REF /OPT:ICF ")
else()
    target_compile_features(${PROJECT_NAME} PUBLIC cxx_final cxx_strong_enums cxx_delegating_constructors
        PRIVATE cxx_std_17)
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall)
endif()

target_include_directories(${PROJECT_NAME}
    PRIVATE "${PROJECT_BINARY_DIR}" "${PROJECT_SOURCE_DIR}/include" "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE ${Boost_LIBRARIES} ${Qt_LIBRARIES} spdlog::spdlog
)
# ==================================================================================================

# === install ======================================================================================
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION "bin"
    LIBRARY DESTINATION "lib"
    ARCHIVE DESTINATION "lib")
# ==================================================================================================
