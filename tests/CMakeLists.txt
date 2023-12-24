if(MSVC)
    # warning C4996: 'fopen': This function or variable may be unsafe. Consider using fopen_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.
    add_definitions(/wd4996)
endif()

macro(sword_add_test name)
    add_executable(test_${name} test_${name}.cpp)
    target_link_libraries(test_${name} PRIVATE sword ${GMP_LIBRARY})
    target_compile_options(test_${name} PRIVATE -std=c++11)

    # add_test(NAME test_${name} COMMAND ${CMAKE_COMMAND} -DTEST_EXECUTABLE=$<TARGET_FILE:test_${name}> -P ${CMAKE_CURRENT_SOURCE_DIR}/../cmake/run_test.cmake)
    add_test(NAME test_${name} COMMAND test_${name} -P ${CMAKE_CURRENT_SOURCE_DIR}/../cmake/run_test.cmake)

    # add test to a virtual project group
    set_property(TARGET test_${name} PROPERTY FOLDER "tests")
endmacro()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/../src)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../cmake)
find_package(GMP)
include_directories(${GMP_INCLUDE_DIR})

sword_add_test(add)
sword_add_test(sub)
sword_add_test(mul)
sword_add_test(div)
sword_add_test(zero_eq)