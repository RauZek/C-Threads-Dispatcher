add_library(c_base_thread STATIC
        base_thread.c
        queue.c
)

target_include_directories(c_base_thread PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
