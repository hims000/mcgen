# mcgen Test Suite

# Collect mcgen source files (excluding main.cpp)
set(MCGEN_LIB_SOURCES
    ${CMAKE_SOURCE_DIR}/src/shared/chunk_data.cpp
    ${CMAKE_SOURCE_DIR}/src/shared/endian.cpp
    ${CMAKE_SOURCE_DIR}/src/shared/gzip_utils.cpp
    ${CMAKE_SOURCE_DIR}/src/shared/mappings/block_mapping.cpp
    ${CMAKE_SOURCE_DIR}/src/core/noise.cpp
    ${CMAKE_SOURCE_DIR}/src/core/terrain_engine.cpp
    ${CMAKE_SOURCE_DIR}/src/core/cave_system.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ore_veins.cpp
    ${CMAKE_SOURCE_DIR}/src/core/structures.cpp
    ${CMAKE_SOURCE_DIR}/src/core/thread_pool.cpp
    ${CMAKE_SOURCE_DIR}/src/java/nbt/nbt_codec.cpp
    ${CMAKE_SOURCE_DIR}/src/java/anvil/region_writer.cpp
    ${CMAKE_SOURCE_DIR}/src/java/world/java_world_writer.cpp
    ${CMAKE_SOURCE_DIR}/src/bedrock/leveldb/bedrock_leveldb.cpp
    ${CMAKE_SOURCE_DIR}/src/bedrock/world/bedrock_world_writer.cpp
    ${CMAKE_SOURCE_DIR}/src/config/world_config.cpp
    ${CMAKE_SOURCE_DIR}/src/config/cli_parser.cpp
)

set(MCGEN_TEST_SOURCES
    test_main.cpp
    test_chunk_data.cpp
    test_endian.cpp
    test_nbt.cpp
    test_noise.cpp
    test_terrain.cpp
    test_mappings.cpp
)

add_executable(mcgen_tests ${MCGEN_TEST_SOURCES} ${MCGEN_LIB_SOURCES})

target_include_directories(mcgen_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(mcgen_tests PRIVATE
    GTest::gtest
    GTest::gtest_main
    Threads::Threads
    ZLIB::ZLIB
)

if(MCGEN_HAS_LEVELDB)
    target_link_libraries(mcgen_tests PRIVATE ${LEVELDB_LIBRARY})
    target_include_directories(mcgen_tests PRIVATE ${LEVELDB_INCLUDE_DIR})
endif()

target_compile_definitions(mcgen_tests PRIVATE
    MCGEN_VERSION="1.0.0"
    MCGEN_LEVELDB_BACKEND="stub"
    MCGEN_HAS_LEVELDB=${MCGEN_HAS_LEVELDB}
    MCGEN_HAS_ZLIBRAW=${MCGEN_HAS_ZLIBRAW}
)

include(GoogleTest)
gtest_discover_tests(mcgen_tests)
