add_executable(test_app
    main.c
)

target_link_libraries(test_app PRIVATE dispatcher)

install(TARGETS test_app RUNTIME DESTINATION bin)
