# Author: Younguk Kim (bluekyu)
# Date  : 2016-08-02

# find boost
option(Boost_USE_STATIC_LIBS    "Boost uses static libraries" OFF)
option(Boost_USE_MULTITHREADED  "Boost uses multithreaded"  ON)
option(Boost_USE_STATIC_RUNTIME "Boost uses static runtime" OFF)

set(BOOST_ROOT "" CACHE PATH "Hint for finding boost root directory")
set(BOOST_INCLUDEDIR "" CACHE PATH "Hint for finding boost include directory")
set(BOOST_LIBRARYDIR "" CACHE PATH "Hint for finding boost library directory")

find_package(Boost 1.62.0 REQUIRED COMPONENTS filesystem system log thread date_time chrono log_setup)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    message(STATUS "Boost include path: ${Boost_INCLUDE_DIRS}")
    if(NOT Boost_USE_STATIC_LIBS)
        set(Boost_LIBRARIES Boost::dynamic_linking ${Boost_LIBRARIES})
    endif()
endif()

# find Qt
set(CMAKE_AUTOUIC ON)               # Run uic automatically when needed.
set(CMAKE_AUTOMOC ON)               # Run moc automatically when needed.
find_package(Qt5Widgets REQUIRED)
find_package(Qt5WebSockets REQUIRED)
set(Qt_LIBRARIES Qt5::Widgets Qt5::WebSockets)

# find rapidjson
set(RAPIDJSON_INCLUDEDIR "" CACHE PATH "Hint for finding rapidjson include directory")

find_package(RapidJSON REQUIRED)
if(RapidJson_FOUND)
    include_directories(${RapidJson_INCLUDE_DIRS})
    message(STATUS "RapidJSON include path: ${RapidJson_INCLUDE_DIRS}")
endif()
