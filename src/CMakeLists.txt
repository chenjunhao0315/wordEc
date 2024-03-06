##############################################

# configure_file(platform.h.in ${CMAKE_CURRENT_BINARY_DIR}/platform.h)

# Add source file to list, and add to special visual folder
function(sword_src_group sword_src_string folder)
    string(REPLACE " " ";" _sword_src_list ${sword_src_string})

    string(REGEX REPLACE "/" "\\\\" _target_folder "${folder}")

    foreach(_file IN LISTS ${_sword_src_list})
        source_group ("${_target_folder}" FILES "${_file}")
    endforeach ()
endfunction()

set(sword_SRCS
    Var.cpp
    Term.cpp
    Monomial.cpp
    Polynomial.cpp
    PolyMgr.cpp

    Rewrite.cpp
)

sword_src_group(sword_SRCS "sources")

# sword amcro
include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/sword_add_module.cmake)

# module implementation
# sword_add_module(aigman)

if(SWORD_SHARED_LIB)
    add_library(sword SHARED ${sword_SRCS})
else()
    add_library(sword STATIC ${sword_SRCS})
endif()
set_target_properties(sword PROPERTIES DEBUG_POSTFIX "d")
if(APPLE OR IOS)
    # macos / ios only accepts a.b.c.d.e where a=24bit b/c/d/e=10bit
    # 20201228 to 20.12.28
    string(SUBSTRING ${SWORD_VERSION} 2 2 SWORD_VERSION_YEAR)
    string(SUBSTRING ${SWORD_VERSION} 4 2 SWORD_VERSION_MONTH)
    string(SUBSTRING ${SWORD_VERSION} 6 2 SWORD_VERSION_DAY)
    set(SWORD_VERSION_APPLE_STRING ${SWORD_VERSION_MAJOR}.${SWORD_VERSION_MINOR}.${SWORD_VERSION_YEAR}.${SWORD_VERSION_MONTH}.${SWORD_VERSION_DAY})
    set_target_properties(sword PROPERTIES VERSION ${SWORD_VERSION_APPLE_STRING} SOVERSION ${SWORD_VERSION_MAJOR})
else()
    set_target_properties(sword PROPERTIES VERSION ${SWORD_VERSION_STRING} SOVERSION ${SWORD_VERSION_MAJOR})
endif()

include(GenerateExportHeader)
generate_export_header(sword)

if(NOT SWORD_SHARED_LIB)
    set_target_properties(sword PROPERTIES COMPILE_FLAGS -DSWORD_STATIC_DEFINE)
endif()

target_include_directories(sword
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include/sword>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)

if(SWORD_OPENMP)
    find_package(OpenMP)
    if(NOT TARGET OpenMP::OpenMP_CXX AND (OpenMP_CXX_FOUND OR OPENMP_FOUND))
        target_compile_options(sword PRIVATE ${OpenMP_CXX_FLAGS})
    endif()
endif()

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../cmake)
find_package(GMP)
include_directories(${GMP_INCLUDE_DIR})
target_link_libraries(sword PUBLIC ${GMP_LIBRARY})

if(SWORD_THREADS)
    set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
    set(THREADS_PREFER_PTHREAD_FLAG TRUE)
    find_package(Threads REQUIRED)

    if(TARGET Threads::Threads)
        target_link_libraries(sword PUBLIC Threads::Threads)
    endif()
endif()

if(WIN32)
    target_compile_definitions(sword PUBLIC NOMINMAX)
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC" OR (CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND CMAKE_CXX_SIMULATE_ID MATCHES "MSVC" AND CMAKE_CXX_COMPILER_FRONTEND_VARIANT MATCHES "MSVC"))
    target_compile_definitions(sword PRIVATE _SCL_SECURE_NO_WARNINGS _CRT_SECURE_NO_DEPRECATE)

    if(CMAKE_BUILD_TYPE MATCHES "(Release|RELEASE|release)")
        target_compile_options(sword PRIVATE /fp:fast)
    endif()

    if(SWORD_SHARED_LIB)
        # msvc argues about stl string and vector uses in exported functions
        target_compile_options(sword PRIVATE /wd4251)
    endif()
else()
    target_compile_options(sword PRIVATE -Wall -Wextra -Wno-unused-function)

    if(NOT SWORD_DISABLE_PIC)
        set_target_properties(sword PROPERTIES POSITION_INDEPENDENT_CODE ON INTERFACE_POSITION_INDEPENDENT_CODE ON)
    endif()

    if(CMAKE_BUILD_TYPE MATCHES "(Release|RELEASE|release)")
        if(NOT CMAKE_SYSTEM_NAME STREQUAL "Emscripten" AND NOT (CMAKE_CXX_COMPILER_ID MATCHES "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.6))
            target_compile_options(sword PRIVATE -Ofast)
        endif()

        target_compile_options(sword PRIVATE -ffast-math)
    endif()

    # target_compile_options(sword PRIVATE -march=native)
    target_compile_options(sword PRIVATE -fvisibility=hidden -fvisibility-inlines-hidden)
    if(SWORD_SHARED_LIB AND SWORD_ENABLE_LTO)
        set_target_properties(sword PROPERTIES INTERPROCEDURAL_OPTIMIZATION ON)
    endif()
endif()

if(SWORD_DISABLE_RTTI)
    target_compile_options(sword PUBLIC -fno-rtti)
endif()

if(SWORD_DISABLE_EXCEPTION)
    target_compile_options(sword PUBLIC -fno-exceptions)
endif()

if(SWORD_COVERAGE)
    target_compile_options(sword PUBLIC -coverage -fprofile-arcs -ftest-coverage)
    target_link_libraries(sword PUBLIC -coverage -lgcov)
endif()

if(SWORD_ASAN)
    target_compile_options(sword PUBLIC -fsanitize=address)
    target_link_libraries(sword PUBLIC -fsanitize=address)
endif()

if(SWORD_DEBUG)
    target_compile_options(sword PRIVATE -g)
endif()

target_compile_options(sword PRIVATE -std=c++11)

# add sword and generate-spirv to a virtual project group
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(TARGET sword PROPERTY FOLDER "libsword")
