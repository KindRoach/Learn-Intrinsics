cmake_minimum_required(VERSION 3.28)
project(LearnSIMD)

set(CMAKE_CXX_STANDARD 20)

if (MSVC)
    add_compile_options(/Qvec-report:2)
else ()
    # Set optimization level to -O3 for RelWithDebInfo builds
    # GCC only automatic vectorization in -O3
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O3 -g")
    set(CMAKE_C_FLAGS_RELWITHDEBINFO "-O3 -g")
    add_compile_options(-fopt-info-vec-optimized)
endif ()

option(ENABLE_SSE "Enable SSE instruction" ON)
option(ENABLE_AVX "Enable AVX instruction" ON)
option(ENABLE_AVX2 "Enable AVX2 instruction" ON)
option(ENABLE_AVX512 "Enable AVX512 instruction" OFF)
option(ENABLE_AMX "Enable AVX512 instruction" OFF)

if (ENABLE_SSE)
    if (MSVC)
        add_compile_options(/arch:SSE4.2)
    else ()
        add_compile_options(-msse4.2)
    endif ()
endif ()

if (ENABLE_AVX)
    if (MSVC)
        add_compile_options(/arch:AVX)
    else ()
        add_compile_options(-mavx)
    endif ()
endif ()

if (ENABLE_AVX2)
    if (MSVC)
        add_compile_options(/arch:AVX2)
    else ()
        add_compile_options(-mavx2)
    endif ()
endif ()

if (ENABLE_AVX512)
    if (MSVC)
        add_compile_options(/arch:AVX512)
    else ()
        add_compile_options(
                -mavx512f
                -mavx512bw
                -mavx512dq
                -mavx512vl
        )
    endif ()
endif ()

add_executable(AutoVectorization auto_vectorization.cpp)
