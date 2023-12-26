if(NOT DEFINED SWORD_VERSION)
    string(TIMESTAMP SWORD_VERSION "%Y%m%d")
endif()

set(SWORD_VERSION_MAJOR 1)
set(SWORD_VERSION_MINOR 0)
set(SWORD_VERSION_PATCH ${SWORD_VERSION})
set(SWORD_VERSION_STRING ${SWORD_VERSION_MAJOR}.${SWORD_VERSION_MINOR}.${SWORD_VERSION_PATCH})
message(STATUS "SWORD_VERSION_STRING = ${SWORD_VERSION_STRING}")

cmake_minimum_required(VERSION 2.8.12)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE release CACHE STRING "Choose the type of build" FORCE)
endif()

if(NOT CMAKE_VERSION VERSION_LESS "3.15")
    # enable CMAKE_MSVC_RUNTIME_LIBRARY
    cmake_policy(SET CMP0091 NEW)
endif()

if(POLICY CMP0025)
    # reference from https://cmake.org/cmake/help/latest/policy/CMP0025.html
    cmake_policy(SET CMP0025 NEW)
endif()

project(sword)

if(MSVC AND NOT CMAKE_VERSION VERSION_LESS "3.15")
    option(SWORD_BUILD_WITH_STATIC_CRT "Enables use of statically linked CRT for statically linked sword" OFF)
    if(SWORD_BUILD_WITH_STATIC_CRT)
        # cmake before version 3.15 not work
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    endif()
endif()

option(SWORD_SHARED_LIB "shared library support" OFF)
option(SWORD_ENABLE_LTO "enable link-time optimization" OFF)
option(SWORD_OPENMP "openmp support" ON)
option(SWORD_INSTALL_SDK "install sword library and headers" ON)
option(SWORD_THREADS "build with threads" ON)
option(SWORD_C_API "build with C api" ON)
option(SWORD_CMAKE_VERBOSE "print verbose cmake messages" OFF)
option(SWORD_DISABLE_PIC "disable position-independent code" OFF)
option(SWORD_BUILD_TESTS "build tests" ON)
option(SWORD_COVERAGE "build for coverage" OFF)
option(SWORD_ASAN "build for address sanitizer" OFF)
option(SWORD_BUILD_BENCHMARK "build benchmark" ON)
option(SWORD_PYTHON "build python api" OFF)
option(SWORD_FORCE_INLINE "force inline some function" ON)
option(SWORD_DISABLE_RTTI "disable rtti" OFF)
option(SWORD_BUILD_TOOLS "build tools" ON)
option(SWORD_BUILD_EXAMPLES "build examples" ON)
option(SWORD_DISABLE_EXCEPTION "disable exception" OFF)
option(SWORD_DEBUG "build with debug mode" ON)

if(SWORD_SHARED_LIB)
    if(SWORD_BUILD_TESTS)
        message(WARNING "SWORD_SHARED_LIB must be OFF to build tests! SWORD_BUILD_TESTS will be turned off.")
        set(SWORD_BUILD_TESTS OFF)
    endif()

    if(SWORD_ENABLE_LTO)
        # enable global link time optimization
        cmake_policy(SET CMP0069 NEW)
        set(CMAKE_POLICY_DEFAULT_CMP0069 NEW)
        include(CheckIPOSupported)
        check_ipo_supported(RESULT ipo_supported OUTPUT ipo_supported_output)
        if(ipo_supported)
            set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
        else()
            message(WARNING "IPO is not supported: ${ipo_supported_output}")
            set(SWORD_ENABLE_LTO OFF)
        endif()
    endif()
endif()

##############################################

include(CheckCXXCompilerFlag)
check_cxx_source_compiles("int main() { int a = 0; asm volatile(\"\" : \"=r\"(a) : \"0\"(a) : \"memory\"); return 0; }" SWORD_COMPILER_SUPPORT_GNU_INLINE_ASM)
if(SWORD_COMPILER_SUPPORT_GNU_INLINE_ASM)
    option(SWORD_GNU_INLINE_ASM "optimize platform with gnu style inline assembly" ON)
else()
    message(WARNING "The compiler does not support gnu style inline assembly. SWORD_GNU_INLINE_ASM will be OFF.")
endif()

if((IOS AND CMAKE_OSX_ARCHITECTURES MATCHES "arm")
    OR (APPLE AND CMAKE_OSX_ARCHITECTURES MATCHES "arm64")
    OR (CMAKE_SYSTEM_PROCESSOR MATCHES "^(arm|aarch64)")
    OR (CMAKE_CXX_COMPILER_ARCHITECTURE_ID MATCHES "ARM64")
    OR ((CMAKE_CXX_COMPILER_ID MATCHES "MSVC" OR (CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND CMAKE_CXX_SIMULATE_ID MATCHES "MSVC" AND CMAKE_CXX_COMPILER_FRONTEND_VARIANT MATCHES "MSVC")) AND (${CMAKE_GENERATOR_PLATFORM} MATCHES "^(arm|arm64)")))
    set(SWORD_TARGET_ARCH arm)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(mips)")
    set(SWORD_TARGET_ARCH mips)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(loongarch64|loongarch32)")
    set(SWORD_TARGET_ARCH loongarch)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(riscv)")
    set(SWORD_TARGET_ARCH riscv)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(powerpc|ppc)")
    set(SWORD_TARGET_ARCH powerpc)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(xtensa)")
    set(SWORD_TARGET_ARCH xtensa)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(s390x)")
    set(SWORD_TARGET_ARCH s390x)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(sw_64)")
    set(SWORD_TARGET_ARCH sw_64)
    #sw_64 is alpha-like platform
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mieee")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mieee")
else()
    set(SWORD_TARGET_ARCH x86)
endif()

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(STATUS "Target arch: ${SWORD_TARGET_ARCH} 64bit")
else()
    message(STATUS "Target arch: ${SWORD_TARGET_ARCH} 32bit")
endif()

##############################################

# set cmake default folder name
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "cmake")

if(CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -s FORCE_FILESYSTEM=1 -s INITIAL_MEMORY=256MB -s EXIT_RUNTIME=1")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s FORCE_FILESYSTEM=1 -s INITIAL_MEMORY=256MB -s EXIT_RUNTIME=1")
    set(CMAKE_EXECUTBLE_LINKER_FLAGS "${CMAKE_EXECUTBLE_LINKER_FLAGS} -s FORCE_FILESYSTEM=1 -s INITIAL_MEMORY=256MB -s EXIT_RUNTIME=1")

    if(SWORD_OPENMP AND SWORD_SIMPLEOMP)
        # TODO better flags for emscripten
        # node --experimental-wasm-threads xxx.js
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -s USE_PTHREADS=1 -s PTHREAD_POOL_SIZE=15")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_PTHREADS=1 -s PTHREAD_POOL_SIZE=15")
        set(CMAKE_EXECUTBLE_LINKER_FLAGS "${CMAKE_EXECUTBLE_LINKER_FLAGS} -s USE_PTHREADS=1 -s PTHREAD_POOL_SIZE=15")
    endif()
endif()

add_subdirectory(src)
# if(SWORD_BUILD_BENCHMARK)
#     add_subdirectory(benchmark)
# endif()
if(SWORD_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
if(SWORD_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
# if(SWORD_PYTHON)
#     add_subdirectory(python)
# endif()

include(ExternalProject)
set_directory_properties(PROPERTIES EP_PREFIX ${CMAKE_BINARY_DIR}/wordEc)
ExternalProject_Add(
    wordEc
    DOWNLOAD_COMMAND ""
    CONFIGURE_COMMAND ""
    BUILD_COMMAND cargo build COMMAND cargo build --release
    BINARY_DIR "${CMAKE_SOURCE_DIR}/wordEc"
    INSTALL_COMMAND ""
    LOG_BUILD ON)
