cmake_minimum_required(VERSION 3.14)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
endif()

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CXX_COMPILR_WARNING_OPTIONS -Wall -Wextra -pedantic)
add_compile_options(${CXX_COMPILR_WARNING_OPTIONS})

project(stlreader LANGUAGES CXX)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
message(STATUS "Set: LIBRARY_OUTPUT_DIRECTORY:${LIBRARY_OUTPUT_DIRECTORY}")
message(STATUS "Set: RUNTIME_OUTPUT_DIRECTORY:${RUNTIME_OUTPUT_DIRECTORY}")

# set(Boost_DEBUG True)
find_package(Boost COMPONENTS numpy python)
if (${Boost_FOUND})
    message(STATUS Boost_VERSION:${Boost_VERSION})
    message(STATUS "Boost_INCLUDE_DIRS:${Boost_INCLUDE_DIRS}")
    message(STATUS "Boost_LIBRARIES:${Boost_LIBRARIES}")
else()
    message(STATUS "Boost Not Found")
endif()

find_package(Python3 COMPONENTS Development)
if (${Python3_FOUND})
    message(STATUS Python3_VERSION:${Python3_VERSION})
    message(STATUS Python3_INCLUDE_DIRS:${Python3_INCLUDE_DIRS})
    message(STATUS Python3_LIBRARIES:${Python3_LIBRARIES})
else()
    message(STATUS "Python3 Not Found")
endif()

add_subdirectory(src)
add_subdirectory(test)
