add_executable(Tests
    TestMain.cpp
)

target_link_libraries(Tests
    GTest::gtest
    GTest::gtest_main
    GTest::gmock
    GTest::gmock_main
)

add_test(NAME Tests COMMAND Tests)

add_custom_target(check
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target test
    DEPENDS Tests
    COMMENT "Building and running tests"
)