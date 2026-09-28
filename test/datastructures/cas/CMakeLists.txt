function(add_seraphic_phoenix_test test_name source_file)
    add_executable(${test_name} ${source_file})
    target_link_libraries(${test_name} PRIVATE seraphic_phoenix)

    add_test(NAME ${test_name}
             COMMAND ${test_name})

    set_tests_properties(${test_name}
        PROPERTIES TIMEOUT 5)
endfunction()

add_seraphic_phoenix_test(TestMPSCSBCreation          test_mpsc_sb_creation.c)
add_seraphic_phoenix_test(TestMPSCSBSimplePush        test_mpsc_sb_simple_push.c)
add_seraphic_phoenix_test(TestMPSCSBSimplePop         test_mpsc_sb_simple_pop.c)
add_seraphic_phoenix_test(TestMPSCSBSimplePushPop     test_mpsc_sb_simple_push_pop.c)
add_seraphic_phoenix_test(TestMPSCSBSimplePeekAdv     test_mpsc_sb_simple_peek_adv.c)
add_seraphic_phoenix_test(TestMPSCSBStressPushPop     test_mpsc_stress_push_pop.c)
add_seraphic_phoenix_test(TestMPSCSBStressPeekAdvance test_mpsc_stress_peek_advance.c)

