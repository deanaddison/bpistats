set(test_srcs bpistats_app_tests.cpp coindesk_analyser_tests.cpp)
set(test_libs bpistats_app)

add_gmock_test(bpistats_tests)
