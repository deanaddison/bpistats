file( GLOB SRCS *.cpp )
add_library(bpistats_app ${SRCS})

target_link_libraries(bpistats_app ${CURL_LIBRARIES} ${Boost_LIBRARIES})

add_subdirectory(test)
