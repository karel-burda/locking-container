![Version](https://img.shields.io/badge/version-0.9.1-blue.svg)
[![License](https://img.shields.io/badge/license-MIT_License-blue.svg?style=flat)](LICENSE)
[![Build Status](https://travis-ci.com/karel-burda/locking-container.svg?branch=develop)](https://travis-ci.com/karel-burda/locking-container)

## Important
This project contains git sub-modules that are needed for building example and tests.

If you just want to use the implementation, you can clone without sub-modules. In case you want to build the example or tests, be sure to clone the repository
with `--recurse-submodules` or `--recursive` on older versions of git. Alternatively, you can clone without sub-modules and initialize these later.

## Introduction
`locking-container` features thread-safe and header-only C++ classes that implement additional thread-safe (using locking mechanism) methods
in addition to the existing ones.
 
It works with all containers from the [STL](https://en.cppreference.com/w/cpp/container).
 
The [read-write lock principle](https://en.wikipedia.org/wiki/Readers%E2%80%93writer_lock) is used, where there might be multiple concurrent readers, but at maximum one writer, is achieved using `std::shared_mutex` (or `std::shared_timed_mutex`) is used.

It simply inherits from the given STL container (be it `std::vector`, `std::forward_list`, `std::unordered_map` with custom hash function and allocators, etc.)
and implements additional methods.

The thread-safety is implemented using either `std::shared_timed_mutex` (in case when C++14 used) or `std::shared_mutex` (in case of C++17 and higher).
The read-only methods uses the read-lock (`std::shared_lock<T>` on the mutex), the "writers" ones use the `std::unique_lock<T>`.

**There are these main classes:**
### `locking_container_basic`
`locking_container_basic` provides methods to run certain block of code (e.g. via lambdas) with read, resp. write, lock:
  * `read_lock(T && action)`
  * `write_lock(T && action)`
  
`locking_container_basic` is not copy-able nor move-able due to the mutex contained within the class.  

Implemented at the [locking_container_basic.hpp](include/locking_container/locking_container_basic.hpp).

#### Example  
> Principle is applicable for any other STL container such as `std::map`, `std::forward_iterator`, and others

```cpp
#include <locking_container/locking_container_basic.hpp>

// use custom allocator or whichever template arguments the container supports
using stl_container_type = std::vector<int>;
burda::stl::locking_container_basic<stl_container_type> locking_container = { 1, 2, 3 };

// the lock itself in the locking_container_basic is used only once
locking_container.write_lock([&]()
{
    // following code runs under the "write" lock, this means that it waits until all "readers"
    // do their work, then acquire a unique lock and perform this code, then it releases the lock
    locking_container.emplace_back(4);
    locking_container.emplace_back(5);
});

// instead of lambda, we could also use binding...
locking_container.read_lock([&]()
{
    // runs under the shared "read" lock, so that there might be multiple concurrent readers, and this
    // waits until at most "writer" at a time does its work (if any)
    const auto size = locking_container.size();
    const auto capacity = locking_container.capacity();
});
```  
### `locking_container`
The class inherits from the above described `locking_container_basic` and implements additional locking variants to all STL containers' methods.
 
This results in implementing each of the container's method variant in a locking manner; the method is named with the `_lock` suffix.
So, for the `std::vector<...>`, we have these:
  * `emplace_back_write_lock(...)`
  * `size_read_lock(...)`
  * `capacity_read_lock(...)`
  * ...

In addition to these existing:
  * `emplace_back(...)`
  * `size(...)`
  * `capacity(...)`
  * ...  
 
So it always adds the `*(read | write)_lock` variant to existing original container's methods. However, the locking versions cannot grant `noexcept` specifier because
of the mutex primitive. 

When using the traditional lock-free methods, there's absolutely no additional overhead, because the base class (the STL container itself) implementation is called.

The are these **important** facts when it comes to usage:
  * `operator[]` is implemented with the read-write lock, so it is always thread-safe and there's no lock-free version provided
  * `begin(), cbegin(), end(), cend(), rbegin(), rcbegin(), rend(), crend()` always use the read-write lock
    * this is due to the `std::begin(...)` and related functions from the `<iterator>` that might be called on the container
    * the `_no_lock(...)` lock-free variants that call STL's implementation are provided

The only header needed resides in [locking_container.hpp](include/locking_container/locking_container.hpp).

#### Example  
> Principle is applicable for any other STL container such as `std::map`, `std::forward_iterator`, and others

```cpp
#include <locking_container/locking_container.hpp>

// use custom allocator or whichever template arguments the container supports
using stl_container_type = std::vector<int>;
burda::stl::locking_container<stl_container_type> locking_container = { 1, 2, 3 };

// note that more that one call of these locking functions in a row is not optimal
// from the performance viewpoint, since it obtains lock multiple times; in this case
// it's better to use "read_lock(...)", resp. "write_lock(...)"
locking_container.emplace_back_write_lock(4);
locking_container.emplace_back_write_lock(99);

// can call original original STL container lock-free methods as well
locking_container.insert(std::end(locking_container), 5);

const auto size = locking_container.size_read_lock();
// above is equivalent of the following:
// locking_container.read_lock([&]()
// {
//     locking_container.size();
// });
```

For full use cases, see implementation of unit tests at [tests/unit](tests/unit).

## Integration
There are two possible headers for inclusion:
```cpp
#include <locking_container/locking_container_basic.hpp>

// or extended version that implements "*_lock" version to every STL method
#include <locking_container/locking_container.hpp> 
```

Implementation resides in the `burda::stl` namespace, so it might be useful to do something like this in your project:
```cpp
template <typename T>
using locking_container = burda::stl::locking_container<T>;
```

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

# this will import search paths, compile definitions and others of the locking_container as well
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
# alternatively assuming that the "locking_container_DIR" variable is set:
# find_package(locking-container CONFIG)

# you can also query (or force specific version during the previous "find_package()" call)
message(STATUS "Found version of locking-container is: ${locking_container_VERSION}")

# this will import search paths, compile definitions and others of the package as well
target_link_libraries(my-project burda::locking_container)
# or with public visibility: target_link_libraries(my-project PUBLIC burda::locking_container)
```

### 2. Manual Way
Not recommended.

Make sure that the `include` directory is in the search paths.

You also have to use at least C++ 14 standard.

## Implementation
The container itself is implemented with the help of pre-processor macros and `auto` type deduction for return values and arguments.

Classes are implemented at the [locking_container_basic.hpp](include/locking_container/locking_container_basic.hpp)
and [locking_container.hpp](include/locking_container/locking_container.hpp).

`locking_container_basic` inherits from `T` where the T might be `std::vector<std::string, my_custom_allocator>`, or `std::unordered_map<int, int, custom_hash, custom_key_comparator>`,
or whichever valid STL container with valid template arguments.

`locking_container` is child of the `locking_container_basic`.
Pre-processor macros are used in order to generate all locking versions of original STL methods, see [macros.hpp](include/locking_container/detail/macros.hpp).

## Unit Tests
Tests require sub-module [cmake-helpers](https://github.com/karel-burda/cmake-helpers).

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

The project is using these jobs:
  * `locking-container, tests -- linux, release with debug info, g++, 64-bit`
  * `locking-container, tests -- osx, release with debug info, clang++, 64-bit`
  * `locking-container, tests -- windows, release, msvc, 32-bit`
