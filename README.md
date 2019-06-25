![Version](https://img.shields.io/badge/version-0.9.0-blue.svg)
[![License](https://img.shields.io/badge/license-MIT_License-blue.svg?style=flat)](LICENSE)
[![Build Status](https://travis-ci.com/karel-burda/locking-container.svg?branch=master)](https://travis-ci.com/karel-burda/locking-container)

## Important
This project contains git sub-modules that are needed for building example and tests.

If you just want to use the implementation, you can clone without sub-modules. In case you want to build the example or tests, be sure to clone the repository
with `--recurse-submodules` or `--recursive` on older versions of git. Alternatively, you can clone without sub-modules and initialize these later.

## Introduction
`locking-container` features a thread-safe and header-only C++ classes that implements additional thread-safe (using locking mechanism) methods
in addition to the existing ones.
 
It works with all containers from the [STL](https://en.cppreference.com/w/cpp/container).
 
The [read-write lock principle](https://en.wikipedia.org/wiki/Readers%E2%80%93writer_lock) where there might be multiple concurrent readers, but at maximum one writer, is achieved using `std::shared_mutex` (or `std::shared_timed_mutex`) is used.

It simply inherits from the given STL container (be it `std::vector`, `std::forward_list`, `std::unordered_map` with custom hash function and allocators, etc.)
and implement additional methods.

The thread-safety is implemented using either `std::shared_timed_mutex` (in case when C++14 used) or `std::shared_mutex` (in case of C++17 and higher).
The read-only methods uses the read-lock (`std::shared_lock<T>` on the mutex), the write-able ones use the `std::unique_lock<T>`.

There are these main classes:
### `locking_container_basic`
`locking_container_basic` provides methods to run certain block of code (e.g. via lambdas) with read, resp. write, lock:
  * `read_lock(T && t)`
  * `write_lock(T && t)`
  
`locked_container_basic` is not copy-able nor move-able due to the mutex contained within the class.  

Implemented at the [locked_container_basic.hpp](include/locking_container/locking_container_basic.hpp).

#### Example  
> Principle is applicable for any other STL container such as `std::map`, `std::forward_iterator`, and others

```cpp
// use custom allocator or whichever template arguments the container supports
using stl_container_type = std::vector<int>;
burda::stl::locking_container_basic<stl_container_type> locking_container = { 1, 2, 3 };

// runs in the same block so that the lock itself in the locking_container is used only once
    
locked_container.write_lock([&]()
{
    // following code runs under the "write" lock, this means that it waits until all "readers"
    // do their work, then acquire a unique lock and perform this code, then it releases the lock
    locked_container.emplace_back(4);
    locked_container.emplace_back(5);
});

// or use function binding instead of lambda...
locking_container.read_lock([&]()
{
    // runs under the "read" lock, so that there might be multiple concurrent readers, and this waits
    // until at most "writer" at a time does its work
    const auto size = locking_container.size();
    const auto capacity = locking_container.capacity();
});
```  
### `locking_container`
The class inherits from the above `locked_container_basic` and implements additional locking variants to all STL containers'.
 
So it results in implementing each of the container's variant with the `_lock` suffix.
So for the `std::vector<...>`, we have these:
  * `emplace_back_write_lock(...)`
  * `size_read_lock(...)`
  * `capacity_read_lock(...)`
  
 
So it always adds the `_lock` variant to existing original container's methods. However, the locking versions cannot grant `noexcept` specifier because
of the mutex primitive. 

When using the traditional lock-free methods, there's absolutely no additional overhead, because the base class (the STL container itself) implementation is called.

The are these **important** facts when it comes to usage:
  * `operator[]` is implemented with the read-write lock, so it is always thread-safe and there's no lock-free version provided
  * `begin(), cbegin(), end(), cend(), rbegin(), rcbegin(), rend(), crend()` always use the read-write lock
    * this is due to the `std::begin(...)` and related functions from the `<iterator>` that might be called on the container
    * the `_no_lock(...)` lock-free variants are provided

The only header needed resides in [locking_container.hpp](include/locking_container/locking_container.hpp).

## Examples

#### Example  
> Principle is applicable for any other STL container such as `std::map`, `std::forward_iterator`, and others

```cpp
// use custom allocator or whichever template arguments the container supports
using stl_container_type = std::vector<int>;
burda::locking_container<stl_container_type> locking_container = { 1, 2, 3 };

// note that more that one usage of these locking functions in a row is not optimal
// from the performance viewpoint, since it obtains lock multiple times; in this case
// it's better to use "read_lock(...)", resp. "write_block(...)"
safe_container.emplace_back_write_lock(4);
safe_container.emplace_back_write_lock(99);

// can call original original STL container lock-free methods as well
safe_container.insert(std::end(safe_container), 5);

const auto size = safe_container.size_safe();
// equivalent of the following (can use functionality from the "locking_container_basic"):
// safe_container.read_lock([&]()
// {
//     safe_container.size();
// });
}
```

For full use cases, see implementation of unit tests at [tests/unit](tests/unit).

## Integration
Implementation resides in the `burda::stl::locking_container` namespace, so it might be useful to do something like
`using locked_container = burda::stl::locked_container` in your project.

### 1. CMake Way
Recommended option.

There are essentially these ways of how to use this package depending on your preferences our build architecture:

#### A) Generate directly
Call `add_subdirectory(...)` directly in your CMakeLists.txt:

```cmake
add_executable(my-project main.cpp)

add_subdirectory(<path-to-locking-container>)
# example: add_subdirectory(ts-container ${CMAKE_BINARY_DIR}/locking-container)

# query of package version
message(STATUS "Current version of the locking_container is: ${locking_container_VERSION}")

add_library(burda::locking_container ALIAS locking_container)

# this will import search paths, compile definitions and other settings of the locking_container as well
target_link_libraries(my-project locking_container)
# or with private visibility: target_link_libraries(my-project PRIVATE locking_container)
```

#### B) Generate separately
Generation phase on the locking-container is run separately, that means that you run:
```cmake
cmake <path-to-locking-container>
# example: cmake -Bbuild/locking-container -Hlocking-container in the root of your project 
```

This will create automatically generated package configuration file `locking_container-config.cmake` that contains exported target and all important information.

Then you can do this in your `CMakeLists.txt`:

```cmake
add_executable(my-project main.cpp)

find_package(locking_container CONFIG PATHS <path-to-binary-dir-of-locking-container>)
# alternatively assuming that the "locking_container_DIR" variable is set: find_package(locking-container CONFIG)

# you can also query (or force specific version during the previous "find_package()" call)
message(STATUS "Found version of locking-container is: ${locking_container_VERSION}")

# this will import search paths, compile definitions and other settings of the locking-container as well
target_link_libraries(my-project burda::locking_container)
# or with public visibility: target_link_libraries(my-project PUBLIC burda::locking_container)
```

### 2. Manual Way
Not recommended.

Make sure that the `include` directory is in the search paths.

You also have to use at least C++ 14 standard.

## Implementation
The container itself is implemented with the help of pre-processor macros and `auto` type deduction for return codes and arguments.

Classes are implemented at the [locking_container_basic.hpp](include/locking_container/locking_container_basic.hpp)
and [locking_container.hpp](include/locking_container/locking_container.hpp).

`locking_container_basic` inherits from `T` where the T might be `std::vector<std::string, my_custom_allocator>`, or `std::unordered_map<int, int, custom_hash, custom_key_comparator>`,
or whichever valid STL container with valid template arguments.

`locking_container` is child of the `locking_container_basic`.

Pre-processor macros are used in order to generator all locking versions of original STL methods, see [macros.hpp](include/locking_container/detail/macros.hpp).

## Unit Tests
Tests require sub-modules [cmake-helpers](https://github.com/karel-burda/cmake-helpers).

For building tests, run CMake in the source directory [tests/unit](tests/unit):

```cmake
cmake -Bbuild -H.


cmake -Bbuild/tests/unit -Htests/unit
      -Dlocking_container_DIR:PATH=$(pwd)/build
cmake --build build/tests/unit

# this runs target "run-all-tests-verbose" that will also run the tests with timeout, etc.:
cmake --build build/tests/unit --target run-all-tests-verbose
```

This is the example of running tests in the debug mode.

For more info, see [.travis.yml](.travis.yml).

## Continuous Integration
Continuous Integration is now being run Linux, OS X and Windows on Travis: https://travis-ci.com/karel-burda/locking-container.

Compilers are set-up with pedantic warning level.
Targets are built in one stage with debug symbols with code coverage measure and in release mode with debug symbols in the second one.

The project is using these jobs:
  * `timers, example, tests -- linux, release with debug info, g++, 64-bit`
  * `timers, example, tests -- osx, release with debug info, clang++, 64-bit`
  * `timers, example, tests -- windows, release, msvc, 32-bit`
