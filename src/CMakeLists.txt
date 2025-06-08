set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

add_library(qwqngx SHARED qwqngx.cpp)
add_library(qwqngx_static qwqngx.cpp)

find_library(QWQNG_LIBRARY NAMES qwqng PATHS /usr/local/lib /usr/local/lib64)
target_link_libraries(qwqngx PRIVATE ${QWQNG_LIBRARY})
target_link_libraries(qwqngx_static PRIVATE ${QWQNG_LIBRARY})

if (CMAKE_GENERATOR MATCHES "Makefiles")
    set_target_properties(qwqngx_static PROPERTIES OUTPUT_NAME qwqngx)
endif ()

install(TARGETS qwqngx qwqngx_static
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

install(FILES qwqngx.h DESTINATION include)