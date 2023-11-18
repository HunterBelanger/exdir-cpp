cmake_minimum_required(VERSION 3.11)

project(exdir-cpp
  VERSION 0.0.1
  LANGUAGES CXX
)

include(CheckCXXCompilerFlag)
include(CMakePackageConfigHelpers)
include(GNUInstallDirs)
include(FetchContent)

#===============================================================================
# Options
option(EXDIR_CPP_SHARED "Build Exdir-CPP as a shared library" OFF)
option(EXDIR_CPP_INSTALL "Install the Exdir-CPP library and header files" ON)
option(EXDIR_CPP_EXAMPLE "Build Exdir-CPP example" OFF)

#===============================================================================
# Get YAML-CPP version 0.8.0
message(STATUS "Downloading yaml-cpp 0.8.0")
FetchContent_Declare(yaml-cpp
  GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
  GIT_TAG        0.8.0
)
if (EXDIR_CPP_SHARED)
  set(YAML_CPP_BUILD_SHARED_LIBS OFF CACHE BOOL "Build yaml-cpp shared library")
endif()
set(YAML_CPP_BUILD_TESTS OFF CACHE BOOL "Enable testing")
set(YAML_CPP_BUILD_TOOLS OFF CACHE BOOL "Enable parse tools")
set(YAML_CPP_BUILD_CONTRIB OFF CACHE BOOL "Enable contrib stuff in library")
if (NOT EXDIR_CPP_INSTALL)
  set(YAML_CPP_INSTALL OFF CACHE BOOL "Enable generation of install target")
endif()
FetchContent_MakeAvailable(yaml-cpp)

#===============================================================================
# Get NDArray
message(STATUS "Downloading NDArray")
FetchContent_Declare(NDArray
  GIT_REPOSITORY https://github.com/HunterBelanger/ndarray.git
  GIT_TAG        master
)
set(NDARRAY_INSTALL OFF CACHE BOOL "Install NDArray")
FetchContent_MakeAvailable(NDArray)

set(EXDIR_CPP_SOURCE_FILES ${EXDIR_CPP_SOURCE_FILES}
  src/object.cpp
  src/group.cpp
  src/raw.cpp
  src/file.cpp
  src/dataset.cpp
)

if (EXDIR_CPP_SHARED)
  add_library(exdir-cpp SHARED ${EXDIR_CPP_SOURCE_FILES})
else()
  add_library(exdir-cpp STATIC ${EXDIR_CPP_SOURCE_FILES})
endif()

# Add alias to make more friendly with FetchConent
add_library(Exdir::exdir-cpp ALIAS exdir-cpp)

target_link_libraries(exdir-cpp PUBLIC yaml-cpp NDArray::NDArray)

target_include_directories(exdir-cpp
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${YAML_CPP_INCLUDE_DIR}>
    $<INSTALL_INTERFACE:include>  
    $<INSTALL_INTERFACE:${YAML_CPP_INCLUDE_DIR}>
)

target_compile_options(exdir-cpp
  PRIVATE
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-W -Wall -Wextra -Wconversion -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W3 /wd4127 /wd4355>
)

target_compile_features(exdir-cpp PUBLIC cxx_std_17)

if (EXDIR_CPP_EXAMPLE)
  add_subdirectory(example)
endif()

set_target_properties(exdir-cpp PROPERTIES
  VERSION "${PROJECT_VERSION}"
  SOVERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}"
  PROJECT_LABEL "exdir-cpp"
)

if(EXDIR_CPP_INSTALL)
  configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/exdir-cppConfig.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/exdir-cppConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/exdir-cpp
  )

  write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/exdir-cppConfigVersion.cmake"
    VERSION "${exdir-cpp_VERSION}"
    COMPATIBILITY SameMinorVersion
  )

  install(TARGETS exdir-cpp yaml-cpp NDArray
          EXPORT exdir-cppTargets
          LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
          ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
          RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
          INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  )

  install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/exdir
          DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
         )

  install(EXPORT exdir-cppTargets
          FILE exdir-cppTargets.cmake
          NAMESPACE Exdir::
          DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/exdir-cpp
  )

  install(FILES
            "${CMAKE_CURRENT_BINARY_DIR}/exdir-cppConfig.cmake"
            "${CMAKE_CURRENT_BINARY_DIR}/exdir-cppConfigVersion.cmake"
          DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/exdir-cpp
  )
endif()
