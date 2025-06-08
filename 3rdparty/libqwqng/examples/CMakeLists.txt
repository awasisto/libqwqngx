option(EXAMPLES "Build example programs" ON)

if (EXAMPLES)
    # Includes
    include_directories( ${CMAKE_CURRENT_BINARY_DIR}
						${CMAKE_CURRENT_SOURCE_DIR}
			)

    message(STATUS "Building example programs.")

    # Source includes
    include_directories(${CMAKE_SOURCE_DIR}/src)

    # Target
    add_executable(randint32 RandInt32.cpp)
    add_executable(randuniform RandUniform.cpp)
    add_executable(randnormal RandNormal.cpp)
    add_executable(randbytes RandBytes.cpp)
    add_executable(clear Clear.cpp)
    add_executable(reset Reset.cpp)
    add_executable(deviceid DeviceId.cpp)
    add_executable(runtimeinfo RuntimeInfo.cpp)
    add_executable(errorhandl ErrorHandling.cpp)
    add_executable(diagnostics Diagnostics.cpp)

    # Dependencies
    target_link_libraries(randint32 qwqng)
    target_link_libraries(randuniform qwqng)
    target_link_libraries(randnormal qwqng)
    target_link_libraries(randbytes qwqng)
    target_link_libraries(clear qwqng)
    target_link_libraries(reset qwqng)
    target_link_libraries(deviceid qwqng)
    target_link_libraries(runtimeinfo qwqng)
    target_link_libraries(errorhandl qwqng)
    target_link_libraries(diagnostics qwqng)

else(EXAMPLES)
    message(STATUS "Not building example programs.")
endif(EXAMPLES)
