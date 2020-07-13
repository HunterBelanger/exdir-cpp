cmake_minimum_required(VERSION 3.12)
project(exdir-cpp VERSION 0.0.1 LANGUAGES CXX)

include(CheckCXXCompilerFlag)
include(GNUInstallDirs)

set(EXDIR_CPP_SOURCE_FILES ${EXDIR_CPP_SOURCE_FILES}
  src/object.cpp
  src/group.cpp
  src/raw.cpp
  src/file.cpp
  src/ndarray.cpp
  src/npy.cpp
  src/dataset.cpp
)

# Add shared library target. Must be shared due to dependence on yaml-cpp.
# I am not completely sure why though.
add_library(exdir-cpp SHARED ${EXDIR_CPP_SOURCE_FILES})

# Find and link with yaml-cpp
find_package (yaml-cpp REQUIRED)
target_link_libraries(exdir-cpp PUBLIC ${YAML_CPP_LIBRARIES})

target_include_directories(exdir-cpp
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${YAML_CPP_INCLUDE_DIR}>
    $<INSTALL_INTERFACE:include>  
    $<INSTALL_INTERFACE:${YAML_CPP_INCLUDE_DIR}>
)

target_compile_options(exdir-cpp
  PRIVATE
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wshadow -Weffc++ -Wno-long-long>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-pedantic -pedantic-errors>
    $<$<CXX_COMPILER_ID:MSVC>:/W3 /wd4127 /wd4355>
)

target_compile_features(exdir-cpp
  PUBLIC
    cxx_std_17  
)

set_target_properties(exdir-cpp PROPERTIES
  VERSION "${PROJECT_VERSION}"
  SOVERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}"
  PROJECT_LABEL "exdir-cpp"
)

install(TARGETS exdir-cpp
    EXPORT exdir-cpp-targets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
		FILES_MATCHING PATTERN "*.hpp"
)
