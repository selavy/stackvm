# tst/CMakeLists.txt

include_directories("${CMAKE_SOURCE_DIR}/include")

set(TESTS
  # add test files here
  t_xl_instruction.cpp
  t_xl_interpreter.cpp
  )

add_executable(
  tests
  main.cpp
  ${TESTS}
  )

target_link_libraries(tests ${CPPUNIT_LIBRARIES})
target_link_libraries(tests xlvm)