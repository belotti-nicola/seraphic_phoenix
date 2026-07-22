function(add_seraphic_phoenix_test test_name source_file)
    add_executable(${test_name} ${source_file})
    target_link_libraries(${test_name} PRIVATE seraphic_phoenix)

    add_test(NAME ${test_name}
             COMMAND ${test_name})

    set_tests_properties(${test_name}
        PROPERTIES TIMEOUT 5)
endfunction()


set(TEST_DATA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/test_data")
set(TEST_DATA_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/test_data")

add_custom_target(copy_test_data ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${TEST_DATA_DIR}"
            "${TEST_DATA_OUTPUT_DIR}"
    COMMENT "Mirroring test data directory to build folder..."
)

# Assicura che i dati vengano copiati PRIMA di eseguire i test
if(TARGET seraphic_phoenix_tests) # nome dell'eseguibile dei tuoi test
    add_dependencies(seraphic_phoenix_tests copy_test_data)
endif()


add_seraphic_phoenix_test(TestInit       sp_init.c)
add_seraphic_phoenix_test(TestSimpleTest sp_simple_test.c)
add_seraphic_phoenix_test(TestCommented  sp_commented.c)