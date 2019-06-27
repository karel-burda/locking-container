cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

project(locking_container VERSION 0.9.1 LANGUAGES CXX)

add_library(${PROJECT_NAME} INTERFACE)

target_sources(${PROJECT_NAME}
                   INTERFACE
                       ${CMAKE_CURRENT_LIST_DIR}/include/${PROJECT_NAME}/locking_container.hpp
                       ${CMAKE_CURRENT_LIST_DIR}/include/${PROJECT_NAME}/locking_container_basic.hpp

                       ${CMAKE_CURRENT_LIST_DIR}/include/${PROJECT_NAME}/detail/macros.hpp
                       ${CMAKE_CURRENT_LIST_DIR}/include/${PROJECT_NAME}/detail/mutex_type.hpp)

target_include_directories(${PROJECT_NAME}
                                INTERFACE
                                    ${CMAKE_CURRENT_LIST_DIR}/include)

target_compile_features(${PROJECT_NAME}
                            INTERFACE
                                cxx_inheriting_constructors
                                cxx_return_type_deduction
                                cxx_trailing_return_types
                                cxx_variadic_templates)

# The project is compatible also with the older C++ 14 (in that case it uses shared_timed_mutex)
include(CheckCXXCompilerFlag)
if (MSVC)
    check_cxx_compiler_flag(/std:c++17 _cxx17_supported)
else()
    check_cxx_compiler_flag(-std=c++17 _cxx17_supported)
endif()

if (_cxx17_supported)
    message(STATUS "Setting C++ 17 standard")
    target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_17)
else()
    message(STATUS "Setting C++ 14 standard")
    target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_14)
endif()

install(TARGETS ${PROJECT_NAME}
        EXPORT _targets)

export(EXPORT _targets
       NAMESPACE burda::
       FILE "${PROJECT_NAME}-config.cmake")

include(CMakePackageConfigHelpers)
write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake
                                 COMPATIBILITY ExactVersion)