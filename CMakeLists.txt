cmake_minimum_required(VERSION 3.28)
project(LearnSIMD)

set(CMAKE_CXX_STANDARD 20)

if (MSVC)
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /Zi")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")
    add_compile_options(/Qvec-report:2)
else ()
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -g")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -g")
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        add_compile_options(-fopt-info-vec-optimized)
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        add_compile_options(-Rpass=loop-vectorize)
    endif ()
endif ()

option(SSE "Enable SSE instruction" OFF)
option(AVX "Enable AVX instruction" OFF)
option(AVX2 "Enable AVX2 instruction" OFF)
option(AVX512 "Enable AVX512 instruction" OFF)
option(AMX "Enable AVX512 instruction" OFF)

if (SSE)
    if (MSVC)
        add_compile_options(/arch:SSE4.2)
    else ()
        add_compile_options(-msse4.2)
    endif ()
endif ()

if (AVX)
    if (MSVC)
        add_compile_options(/arch:AVX)
    else ()
        add_compile_options(-mavx)
    endif ()
endif ()

if (AVX2)
    if (MSVC)
        add_compile_options(/arch:AVX2)
    else ()
        add_compile_options(-mavx2)
    endif ()
endif ()

if (AVX512)
    if (MSVC)
        add_compile_options(/arch:AVX512)
    else ()
        add_compile_options(
                -mavx512f
                -mavx512fp16
                -mavx512bf16
                -mavx512vnni
        )
    endif ()
endif ()

if (AMX)
    if (MSVC)
        message(FATAL_ERROR "MSVC does not support AMX currently")
    else ()
        add_compile_options(
                -mamx-tile
                -mamx-int8
                -mamx-bf16
        )
    endif ()

    add_executable(AmxExample amx_example.cpp)
endif ()

add_executable(AutoVectorization auto_vectorization.cpp)
