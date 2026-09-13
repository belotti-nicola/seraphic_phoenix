function(add_seraphic_phoenix_test test_name source_file)
    add_executable(${test_name} ${source_file})
    target_link_libraries(${test_name} PRIVATE seraphic_phoenix)

    add_test(NAME ${test_name}
             COMMAND ${test_name})

    set_tests_properties(${test_name}
        PROPERTIES TIMEOUT 5)
endfunction()

add_seraphic_phoenix_test(TestRB test_sp_rb.c)
