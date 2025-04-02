set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
add_executable(Tests
    TestMain.cpp
    ${SRC_DIR}/Network.cpp
    ${SRC_DIR}/Task.cpp
    ${SRC_DIR}/System.cpp
)

target_include_directories(Tests PUBLIC "${SRC_DIR}/include")

target_link_libraries(Tests
    GTest::gtest
    GTest::gtest_main
    GTest::gmock
    GTest::gmock_main
)

add_custom_target(run_tests
    COMMAND $<TARGET_FILE:Tests>
    DEPENDS Tests
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running tests"
)