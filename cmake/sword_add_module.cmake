macro(sword_add_module class)
    string(TOLOWER ${class} name)

    # WITH_LAYER_xxx option
    if(${ARGC} EQUAL 2)
        option(WITH_LAYER_${name} "build with module ${name}" ${ARGV1})
    else()
        option(WITH_LAYER_${name} "build with module ${name}" ON)
    endif()

    if(SWORD_CMAKE_VERBOSE)
        message(STATUS "WITH_LAYER_${name} = ${WITH_LAYER_${name}}")
    endif()

    if(WITH_LAYER_${name})
        list(APPEND sword_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/module/${name}.cpp)

        # look for arch specific implementation and append source
        # optimized implementation for armv7, aarch64 or x86
        set(LAYER_ARCH_SRC ${CMAKE_CURRENT_SOURCE_DIR}/module/${SWORD_TARGET_ARCH}/${name}_${SWORD_TARGET_ARCH}.cpp)
        if(EXISTS ${LAYER_ARCH_SRC})
            set(WITH_LAYER_${name}_${SWORD_TARGET_ARCH} 1)
            list(APPEND sword_SRCS ${LAYER_ARCH_SRC})
        endif()

        set(LAYER_VULKAN_SRC ${CMAKE_CURRENT_SOURCE_DIR}/module/vulkan/${name}_vulkan.cpp)
        if(SWORD_VULKAN AND EXISTS ${LAYER_VULKAN_SRC})
            set(WITH_LAYER_${name}_vulkan 1)
            list(APPEND sword_SRCS ${LAYER_VULKAN_SRC})
        endif()
    endif()
endmacro()