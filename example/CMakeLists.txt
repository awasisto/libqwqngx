include_directories(${CMAKE_SOURCE_DIR}/src)
add_executable(qwqngx-example main.cpp)
target_link_libraries(qwqngx-example PRIVATE qwqngx)