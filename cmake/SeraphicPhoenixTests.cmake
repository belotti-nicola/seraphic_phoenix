function(add_seraphic_phoenix_test test_name source_file)
    add_executable(${test_name} ${source_file})

    target_include_directories(sp_test_utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

    target_link_libraries(${test_name} PRIVATE seraphic_phoenix sp::test_utils)

    add_test(NAME ${test_name} COMMAND ${test_name})
    set_tests_properties(${test_name} PROPERTIES TIMEOUT 5)
endfunction()

function(add_seraphic_phoenix_tests)
    foreach(source_file IN LISTS ARGN)
        get_filename_component(base_name "${source_file}" NAME_WE)
        add_seraphic_phoenix_test("Test_${base_name}" "${source_file}")
    endforeach()
endfunction()