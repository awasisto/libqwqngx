# Includes
include_directories( ${CMAKE_CURRENT_BINARY_DIR}
                     ${CMAKE_CURRENT_SOURCE_DIR}
                     )

# Targets
set(cpp_sources     qwqng.cpp)
set(cpp_headers     qwqng.hpp)

add_library(qwqng SHARED ${cpp_sources})

set_target_properties(qwqng PROPERTIES VERSION ${MAJOR_VERSION}.${MINOR_VERSION}.6 SOVERSION 1)

# Static library
add_library(qwqng-static STATIC ${cpp_sources})
set_target_properties(qwqng-static PROPERTIES OUTPUT_NAME "qwqng")

# Prevent clobbering each other during the build
set_target_properties(qwqng PROPERTIES CLEAN_DIRECT_OUTPUT 1)
set_target_properties(qwqng-static PROPERTIES CLEAN_DIRECT_OUTPUT 1)

# Dependencies
target_link_libraries(qwqng ${LIBFTDI_LIBRARIES})
target_link_libraries(qwqng-static ${LIBFTDI_LIBRARIES})

# Install
if(${UNIX})

   install( TARGETS qwqng
            LIBRARY DESTINATION lib${LIB_SUFFIX}
            COMPONENT sharedlibs
            )

   install( TARGETS qwqng-static
            ARCHIVE DESTINATION lib${LIB_SUFFIX}
            COMPONENT staticlibs
            )

   install( FILES ${cpp_headers}
            DESTINATION include
            COMPONENT headers
            )

endif(${UNIX})