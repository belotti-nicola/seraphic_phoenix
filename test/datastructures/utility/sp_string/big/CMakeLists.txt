add_seraphic_phoenix_test(
    TestSPStringCreate
    test_sps_create_big.c
)

add_seraphic_phoenix_test(
    TestSPStringNCreate
    test_sps_ncreate_big.c
)

add_seraphic_phoenix_test(
    TestSPStringCapacity
    test_sps_capacity_big.c
)

add_seraphic_phoenix_test(
    TestSPStringAppendBigNoRealloc
    test_sps_append_big_no_realloc.c
)

add_seraphic_phoenix_test(
    TestSPStringAppendBigRealloc
    test_sps_append_big_realloc.c
)

add_seraphic_phoenix_test(
    TestSPStringSizeBig
    test_sps_size_big.c
)