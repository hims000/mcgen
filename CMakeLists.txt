cmake_minimum_required(VERSION 3.16)
project(mcgen VERSION 1.0.0 LANGUAGES CXX)

# C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Options
option(MCGEN_BUILD_TESTS "Build tests" ON)
option(MCGEN_BUILD_DOCS "Build documentation" OFF)
option(MCGEN_FORCE_STUB "Force stub LevelDB backend" OFF)

# Compiler warnings
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(
        -Wall -Wextra -Wpedantic
        -Wconversion -Wsign-conversion
        -Wshadow -Wnon-virtual-dtor
    )
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    add_compile_options(/W4 /permissive-)
endif()

# Find packages
find_package(Threads REQUIRED)
find_package(ZLIB REQUIRED)

# Find LevelDB (with ZlibRaw support detection)
set(MCGEN_HAS_LEVELDB 0)
set(MCGEN_HAS_ZLIBRAW 0)
set(MCGEN_LEVELDB_BACKEND "stub")

if(NOT MCGEN_FORCE_STUB)
    find_library(LEVELDB_LIBRARY
        NAMES leveldb leveldb_mcpe
        PATHS /usr/local/lib /usr/lib /opt/local/lib
        DOC "LevelDB library"
    )

    find_path(LEVELDB_INCLUDE_DIR
        NAMES leveldb/db.h
        PATHS /usr/local/include /usr/include /opt/local/include
        DOC "LevelDB headers"
    )

    if(LEVELDB_LIBRARY AND LEVELDB_INCLUDE_DIR)
        set(MCGEN_HAS_LEVELDB 1)
        set(MCGEN_LEVELDB_BACKEND "system")

        # Check for ZlibRaw support
        if(EXISTS "${LEVELDB_INCLUDE_DIR}/leveldb/zlib_compressor.h")
            set(MCGEN_HAS_ZLIBRAW 1)
            message(STATUS "✅ Found leveldb-mcpe with ZlibRaw support")
        else()
            message(WARNING "⚠️ Found LevelDB but no ZlibRaw support. "
                "Bedrock output may be incompatible with Minecraft.")
        endif()

        message(STATUS "LevelDB library: ${LEVELDB_LIBRARY}")
        message(STATUS "LevelDB include: ${LEVELDB_INCLUDE_DIR}")
    else()
        message(STATUS "ℹ️ LevelDB not found, using stub backend")
    endif()
else()
    message(STATUS "ℹ️ Force stub backend requested")
endif()

# Source files
set(MCGEN_SOURCES
    src/main.cpp
    src/shared/chunk_data.cpp
    src/shared/endian.cpp
    src/shared/gzip_utils.cpp
    src/shared/mappings/block_mapping.cpp
    src/core/terrain_engine.cpp
    src/core/noise.cpp
    src/core/cave_system.cpp
    src/core/ore_veins.cpp
    src/core/structures.cpp
    src/core/thread_pool.cpp
    src/java/nbt/nbt_codec.cpp
    src/java/anvil/region_writer.cpp
    src/java/world/java_world_writer.cpp
    src/bedrock/world/bedrock_world_writer.cpp
    src/config/world_config.cpp
    src/config/cli_parser.cpp
)

# Bedrock LevelDB source (contains both real and stub implementations)
list(APPEND MCGEN_SOURCES src/bedrock/leveldb/bedrock_leveldb.cpp)

# Main executable
add_executable(mcgen ${MCGEN_SOURCES})

target_include_directories(mcgen PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Link libraries
target_link_libraries(mcgen PRIVATE
    Threads::Threads
    ZLIB::ZLIB
)

if(MCGEN_HAS_LEVELDB)
    target_link_libraries(mcgen PRIVATE ${LEVELDB_LIBRARY})
    target_include_directories(mcgen PRIVATE ${LEVELDB_INCLUDE_DIR})
endif()

# Compile definitions
target_compile_definitions(mcgen PRIVATE
    MCGEN_VERSION="${PROJECT_VERSION}"
    MCGEN_HAS_LEVELDB=${MCGEN_HAS_LEVELDB}
    MCGEN_HAS_ZLIBRAW=${MCGEN_HAS_ZLIBRAW}
    MCGEN_LEVELDB_BACKEND="${MCGEN_LEVELDB_BACKEND}"
)

# Tests
if(MCGEN_BUILD_TESTS)
    find_package(GTest)
    if(GTest_FOUND)
        enable_testing()
        add_subdirectory(tests)
    else()
        message(STATUS "ℹ️ GTest not found, skipping tests")
    endif()
endif()

# Print configuration summary
message(STATUS "")
message(STATUS "mcgen Configuration Summary:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  LevelDB Backend: ${MCGEN_LEVELDB_BACKEND}")
message(STATUS "  ZlibRaw Support: ${MCGEN_HAS_ZLIBRAW}")
message(STATUS "  Tests: ${MCGEN_BUILD_TESTS}")
message(STATUS "")
