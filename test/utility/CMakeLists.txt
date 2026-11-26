add_library(sp_test_utils STATIC 
    src/string_gen.c
    src/string_builder.c
    src/string_recipe.c
)

add_library(sp::test_utils ALIAS sp_test_utils)

target_include_directories(sp_test_utils PUBLIC include)
target_compile_features(sp_test_utils PUBLIC c_std_99)