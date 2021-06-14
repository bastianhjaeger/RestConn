set(SRC_FILES
  AllTests.cpp
  test_base.cpp)

add_executable(${PROJECT_NAME}.test
  ${SRC_FILES})
target_include_directories(${PROJECT_NAME}.test
  PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
         ${testicpp_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME}.test
  ${PROJECT_NAME} stdc++fs gtest pthread)
target_compile_definitions(${PROJECT_NAME}.test 
  PRIVATE TEST_DIR="${CMAKE_CURRENT_LIST_DIR}/test")

install(TARGETS ${PROJECT_NAME}.test
        DESTINATION "${tool_dest}")
