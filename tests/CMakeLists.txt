include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

add_executable(core_tests test_version.cpp)

target_link_libraries(core_tests PRIVATE core GTest::gtest_main)

set_project_warnings(core_tests)

include(GoogleTest)
gtest_discover_tests(core_tests)