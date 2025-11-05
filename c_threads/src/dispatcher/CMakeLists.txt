add_library(dispatcher SHARED
        dispatcher.c
)

target_include_directories(dispatcher PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(dispatcher PRIVATE
                      c_base_thread
                      c_simple_process
                      c_advanced_process
                      dl
)

install(TARGETS dispatcher
        LIBRARY DESTINATION lib
)
