function(add_seraphic_phoenix_test test_name source_file)
    add_executable(${test_name} ${source_file})
    target_link_libraries(${test_name} PRIVATE seraphic_phoenix)

    add_test(NAME ${test_name}
             COMMAND ${test_name})

    set_tests_properties(${test_name}
        PROPERTIES TIMEOUT 5)
endfunction()

add_seraphic_phoenix_test(TestMPSCSBCreation test_mpsc_sb_creation.c)
add_seraphic_phoenix_test(TestMPSCSBSimple   test_mpsc_sb_simple.c)
add_seraphic_phoenix_test(TestMPSCSBStress   test_mpsc_stress.c)

