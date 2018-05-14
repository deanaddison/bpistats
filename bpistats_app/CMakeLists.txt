file( GLOB SRCS *.cpp )
add_library(bpistats_app ${SRCS})

target_include_directories(bpistats_app PRIVATE ${JSONCPP_INCLUDE_DIRS})
target_link_libraries(bpistats_app ${Boost_LIBRARIES} ${JSONCPP_LIBRARIES})

add_subdirectory(test)
