add_library(dispatcher SHARED
        dispatcher.c
)

target_include_directories(dispatcher PUBLIC ${CMAKE_SOURCE_DIR})

target_link_libraries(dispatcher PRIVATE c_base_thread)

install(TARGETS dispatcher
        LIBRARY DESTINATION lib
)
