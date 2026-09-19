add_library(seraphic_phoenix STATIC
    datastructures/sp_queue.c
    datastructures/sp_ring_buffer.c
    datastructures/cas/mpsc_stream_buffer.c
    datastructures/utility/safe_linear_buffer.c
    formats/sp_csvreader.c
    utils/sp_utils.c
)

target_include_directories(seraphic_phoenix
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        # Visibile SOLO durante la compilazione dei file .c in src/
        ${CMAKE_SOURCE_DIR}/include/internal
)