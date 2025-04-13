cmake_minimum_required(VERSION 3.28)
project(LearnSIMD)

set(CMAKE_CXX_STANDARD 20)

if (MSVC)
    add_compile_options(/Qvec-report:2)
else ()
    add_compile_options(-fopt-info-vec-optimized)
endif ()

option(ENABLE_AVX "Enable AVX instruction" ON)
option(ENABLE_AVX2 "Enable AVX2 instruction" ON)
option(ENABLE_AVX512 "Enable AVX512 instruction" ON)
option(ENABLE_AMX "Enable AVX512 instruction" ON)

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

add_executable(LearnSIMD main.cpp)
