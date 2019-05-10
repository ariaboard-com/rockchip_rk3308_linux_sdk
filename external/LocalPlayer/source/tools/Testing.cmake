if(POLICY CMP0057)
    cmake_policy(SET CMP0057 NEW)
endif()

include(CTest)
include(ThirdParty/googletest.cmake)

add_custom_target(unit COMMAND ${CMAKE_CTEST_COMMAND})

macro(discover_unit_tests includes libraries)
    # This will result in some errors not finding GTest when running cmake, but allows us to better integrate with CTest
    find_package(GTest ${GTEST_PACKAGE_CONFIG})
    if(BUILD_TESTING)
        set (extra_macro_args ${ARGN})
        LIST(LENGTH extra_macro_args num_extra_args)
        if (${num_extra_args} GREATER 0)
            list(GET extra_macro_args 0 inputs)
        endif()
        file(GLOB_RECURSE tests RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/*Test.cpp")
        foreach (testsourcefile IN LISTS tests)
            get_filename_component(testname ${testsourcefile} NAME_WE)
            add_executable(${testname} ${testsourcefile})
            target_include_directories(${testname} PRIVATE ${includes})
            target_link_libraries(${testname} ${libraries} gtest_main gmock_main)
            GTEST_ADD_TESTS(${testname} "${inputs}" ${testsourcefile})
            add_dependencies(unit ${testname})
        endforeach ()
     endif()
endmacro()

option(ACSDK_EXCLUDE_TEST_FROM_ALL "Exclude unit test from all." OFF)

macro(acsdk_add_test_subdirectory_if_allowed)
    if (ACSDK_EXCLUDE_TEST_FROM_ALL)
        add_subdirectory("test" EXCLUDE_FROM_ALL)
    else()
        add_subdirectory("test")
    endif()
endmacro()
