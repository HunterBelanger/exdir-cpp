cmake_minimum_required(VERSION 3.9)
project(exdir CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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

add_library(exdir_shared SHARED $<TARGET_OBJECTS:exdir>)
add_library(exdir_static STATIC $<TARGET_OBJECTS:exdir>)

add_subdirectory(vendor/yaml-cpp)

target_link_libraries(exdir_shared PUBLIC yaml-cpp_shared)
target_link_libraries(exdir_static PUBLIC yaml-cpp_static)

set_target_properties(exdir_shared PROPERTIES OUTPUT_NAME exdir)
set_target_properties(exdir_static PROPERTIES OUTPUT_NAME exdir)

add_executable(test example/main.cpp)
target_link_libraries(test PUBLIC exdir_shared)

install(TARGETS exdir_shared exdir_static
        LIBRARY
          DESTINATION lib
          COMPONENT Libraries
          NAMELINK_COMPONENT Development
        PUBLIC_HEADER
          DESTINATION include
          COMPONENT Development
)

install(TARGETS exdir
        PUBLIC_HEADER
          DESTINATION include/exdir
)