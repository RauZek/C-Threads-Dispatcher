add_library(c_advanced_process SHARED
        advanced_process.c
)

target_include_directories(c_advanced_process PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(c_advanced_process PRIVATE c_base_thread)

install(TARGETS c_advanced_process
        LIBRARY DESTINATION lib
)
