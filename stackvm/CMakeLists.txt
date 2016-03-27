# stackvm/CMakeLists.txt

add_executable(
  stackvm
  command_line.cpp
  main.cpp
  )

#target_link_libraries(stackvm libname)

target_link_libraries(stackvm ${Boost_LIBRARIES})
