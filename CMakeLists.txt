cmake_minimum_required(VERSION 3.12)

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VERSION 0.0.${PATCH_VERSION})

project(homework_02 VERSION ${PROJECT_VERSION})

option(WITH_BOOST_TEST "Whether to build Boost test" ON)

configure_file(version.h.in version.h)


add_library(homework_02 lib.cpp)
target_include_directories(homework_02 PUBLIC "${CMAKE_BINARY_DIR}")


add_executable(homework_02_cli ip_filter.cpp)
target_link_libraries(homework_02_cli PRIVATE homework_02)

set_target_properties(homework_02_cli homework_02 PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
)

if(WITH_BOOST_TEST)
    find_package(Boost COMPONENTS unit_test_framework REQUIRED)
    add_executable(test_version test_version.cpp)

    set_target_properties(test_version PROPERTIES
        CXX_STANDARD 14
        CXX_STANDARD_REQUIRED ON
    )

    target_include_directories(test_version PRIVATE ${Boost_INCLUDE_DIR})
    target_compile_definitions(test_version PRIVATE BOOST_TEST_DYN_LINK)

    target_link_libraries(test_version
        ${Boost_LIBRARIES}
        homework_02
    )
endif()

if (MSVC)
    target_compile_options(homework_02_cli PRIVATE /W4)
    target_compile_options(homework_02 PRIVATE /W4)
    if(WITH_BOOST_TEST)
        target_compile_options(test_version PRIVATE /W4)
    endif()
else ()
    target_compile_options(homework_02_cli PRIVATE -Wall -Wextra -pedantic )
    target_compile_options(homework_02 PRIVATE -Wall -Wextra -pedantic )
    if(WITH_BOOST_TEST)
        target_compile_options(test_version PRIVATE -Wall -Wextra -pedantic )
    endif()
endif()

install(TARGETS homework_02_cli RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT example@example.com)
include(CPack)

if(WITH_BOOST_TEST)
    enable_testing()
    add_test(test_version test_version)
endif()
