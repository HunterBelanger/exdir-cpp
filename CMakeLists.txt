cmake_minimum_required(VERSION 3.5)
project(exdir CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_STATIC_LIBS "BUild only static libs" OFF)
option(BUILD_EXAMPLE "Build example executable" ON)
option(BUILD_TESTS "Build unit tests" OFF)

if(MSVC)
  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
  set(BUILD_STATIC OFF CACHE BOOL "Build static lib")
  set(BUILD_SHARED ON CACHE BOOL "Build shared lib")
elseif(BUILD_SHARED_LIBS)
  set(BUILD_STATIC OFF CACHE BOOL "Build static lib")
  set(BUILD_SHARED ON CACHE BOOL "Build shared lib")
elseif(BUILD_STATIC_LIBS)
  set(BUILD_STATIC ON CACHE BOOL "Build static lib")
  set(BUILD_SHARED OFF CACHE BOOL "Build shared lib")
else()
  set(BUILD_STATIC ON CACHE BOOL "Build static lib")
  set(BUILD_SHARED ON CACHE BOOL "Build shared lib") 
endif()

include_directories(include vendor/yaml-cpp/include)

set(EXDIR_SOURCE_FILES ${EXDIR_SOURCE_FILES}
  src/object.cpp
  src/group.cpp
  src/raw.cpp
  src/file.cpp
  src/array.cpp
  src/npy.cpp
  src/dataset.cpp
)

set(EXDIR_HEADER_FILES ${EXDIR_HEADER_FILES}
  include/exdir/object.hpp
  include/exdir/group.hpp
  include/exdir/raw.hpp
  include/exdir/file.hpp
  include/exdir/array.hpp
  include/exdir/npy.hpp
  include/exdir/dataset.hpp
  include/exdir/exdir.hpp
)

add_library(exdir OBJECT ${EXDIR_SOURCE_FILES})
set_target_properties(exdir PROPERTIES PUBLIC_HEADER "${EXDIR_HEADER_FILES}")

set_property(TARGET exdir PROPERTY POSITION_INDEPENDENT_CODE 1)

add_subdirectory(vendor/yaml-cpp)

if(BUILD_SHARED)
  add_library(exdir_shared SHARED $<TARGET_OBJECTS:exdir>)
  target_link_libraries(exdir_shared PUBLIC yaml-cpp_shared)
  set_target_properties(exdir_shared PROPERTIES OUTPUT_NAME exdir)
endif()

if(BUILD_STATIC)
  add_library(exdir_static STATIC $<TARGET_OBJECTS:exdir>)
  target_link_libraries(exdir_static PUBLIC yaml-cpp_static)
  set_target_properties(exdir_static PROPERTIES OUTPUT_NAME exdir)
endif()

if(BUILD_EXAMPLE)
  add_executable(example example/main.cpp)
  if(BUILD_SHARED)
    target_link_libraries(example PUBLIC exdir_shared)
  else()
    target_link_libraries(example PUBLIC exdir_static)
  endif()
endif()

if(BUILD_TESTS)
  find_package(GTest REQUIRED)
  if(GTest_FOUND)
    add_subdirectory(tests)
  endif()
endif()

if(NOT MSVC)
  if(BUILD_SHARED)
    install(TARGETS exdir_shared
            LIBRARY
              DESTINATION lib
              COMPONENT Libraries
              NAMELINK_COMPONENT Development
            PUBLIC_HEADER
              DESTINATION include
              COMPONENT Development
    )
  endif()

  if(BUILD_STATIC)
    install(TARGETS exdir_static
            LIBRARY
              DESTINATION lib
              COMPONENT Libraries
              NAMELINK_COMPONENT Development
            PUBLIC_HEADER
              DESTINATION include
              COMPONENT Development
    )
  endif()

  install(DIRECTORY "include/exdir" DESTINATION include)
endif()
