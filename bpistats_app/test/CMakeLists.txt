set(test_srcs bpistats_app_tests.cpp coindesk_analyser_tests.cpp)
set(test_libs bpistats_app)

configure_file(test.json ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test.json COPYONLY)

add_gmock_test(bpistats_tests)
