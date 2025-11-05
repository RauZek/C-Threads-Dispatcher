add_library(c_simple_process SHARED
        simple_process.c
)

target_include_directories(c_simple_process PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(c_simple_process PRIVATE c_base_thread)

install(TARGETS c_simple_process
        LIBRARY DESTINATION lib
)
