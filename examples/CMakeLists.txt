macro(sword_add_example name)
    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} PRIVATE sword ${GMP_LIBRARY})
    target_compile_options(${name} PRIVATE -std=c++11)

    # add test to a virtual project group
    set_property(TARGET ${name} PROPERTY FOLDER "examples")
endmacro()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/../src)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../cmake)
find_package(GMP)
include_directories(${GMP_INCLUDE_DIR})

# sword_add_example(zero_eq)
#sword_add_example(zero_eq_cl)
#sword_add_example(zero_check)
sword_add_example(rewrite)
