# tst/CMakeLists.txt

include_directories("${CMAKE_SOURCE_DIR}/include")

set(TESTS
  # add test files here
  t_xl_instruction.cpp
  )

add_executable(
  tests
  main.cpp
  ${TESTS}
  )

target_link_libraries(tests ${CPPUNIT_LIBRARIES})
# link with your libraries
#target_link_libraries(tests library-name)