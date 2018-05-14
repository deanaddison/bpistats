cmake_minimum_required(VERSION 3.10)

project(dma CXX)

set(CMAKE_CXX_STANDARD 17)
add_compile_options("-fdiagnostics-color=auto")

set(warnings "-Wno-deprecated-declarations -Wall -Wextra -Werror -Wpedantic")
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} ${warnings})

# find packages
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
include(cmake/add_gmock_test.cmake)

set(MIN_BOOST_VERSION "1.66.0")
set(Boost_USE_STATIC_LIBS OFF)
#set(Boost_USE_MULTITHREAD OFF)

find_package(Boost ${MIN_BOOST_VERSION}
        REQUIRED
        COMPONENTS
        date_time
        filesystem
        program_options
        system
        )

if(Boost_FOUND)

    message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
    message(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")
    message(STATUS "Boost_VERSION: ${Boost_VERSION}")

    include_directories(${Boost_INCLUDE_DIRS})

endif()

find_package(PkgConfig REQUIRED)
pkg_search_module(JSONCPP REQUIRED jsoncpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

include(cmake/googletest.cmake)

get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
    message(STATUS "-I ${dir}")
endforeach()

enable_testing()

add_subdirectory(bpistats_app)

add_executable(bpistats main.cpp)

if(Boost_FOUND)

    target_link_libraries(bpistats bpistats_app ${Boost_LIBRARIES} ${JSONCPP_LIBRARIES})

endif()
