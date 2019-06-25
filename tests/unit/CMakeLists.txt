cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

project(locking_container_tests LANGUAGES CXX)

include(${CMAKE_CURRENT_LIST_DIR}/../../submodules/cmake-helpers/cmake-helpers/cpp_gtest.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/../../submodules/cmake-helpers/cmake-helpers/cpp_warnings.cmake)

add_executable(${PROJECT_NAME} "")

target_sources(${PROJECT_NAME}
                   PRIVATE
                       ${CMAKE_CURRENT_LIST_DIR}/include/locking_container_test.hpp
                       ${CMAKE_CURRENT_LIST_DIR}/src/locking_container_vector_test.cpp)

target_include_directories(${PROJECT_NAME}
                                PRIVATE
                                    ${CMAKE_CURRENT_LIST_DIR}/include)

find_package(locking_container CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE burda::locking_container)

target_compile_options(${PROJECT_NAME}
                           PRIVATE
                               $<$<CXX_COMPILER_ID:MSVC>:/MP>)

burda_cmake_helpers_cpp_gtest_bootstrap_and_link(${PROJECT_NAME} "release-1.8.1" "Release" PRIVATE)

enable_testing()

add_test(NAME all-unit-tests
         COMMAND $<TARGET_FILE:${PROJECT_NAME}> --gtest_color=yes --gtest_shuffle)

add_custom_target(run-all-tests-verbose
                  COMMAND ${CMAKE_CTEST_COMMAND} -V -C ${CMAKE_BUILD_TYPE} --build-run-dir ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} --timeout 300
                  DEPENDS ${PROJECT_NAME})

burda_cmake_helpers_cpp_warnings_add_pedantic_level_cxx(${PROJECT_NAME} PRIVATE CXX)
