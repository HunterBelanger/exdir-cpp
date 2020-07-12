cmake_minimum_required(VERSION 3.5)
project(exdir-cpp VERSION 0.0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CMakePackageConfigHelpers)
include(CMakeDependentOption)
include(CheckCXXCompilerFlag)
include(GNUInstallDirs)
include(CTest)

set(not-msvc $<NOT:$<CXX_COMPILER_ID:MSVC>>)

include_directories(include)

set(EXDIR_CPP_SOURCE_FILES ${EXDIR_CPP_SOURCE_FILES}
  src/object.cpp
  src/group.cpp
  src/raw.cpp
  src/file.cpp
  src/ndarray.cpp
  src/npy.cpp
  src/dataset.cpp
)

set(exdir-cpp-lib-type STATIC)
if(BUILD_SHARED_LIBS)
  set(exdir-cpp-lib-type SHARED)
endif()

add_library(exdir-cpp ${exdir-cpp-lib-type} ${EXDIR_CPP_SOURCE_FILES})

# Find and link with yaml-cpp
find_package (yaml-cpp REQUIRED)
include_directories(${YAML_CPP_INCLUDE_DIR})
target_link_libraries (exdir-cpp ${YAML_CPP_LIBRARIES})

target_compile_options(exdir-cpp
  PRIVATE
    $<${not-msvc}:-Wall -Wextra -Wshadow -Weffc++ -Wno-long-long>
    $<${not-msvc}:-pedantic -pedantic-errors>
    
    $<$<CXX_COMPILER_ID:MSVC>:/W3 /wd4127 /wd4355>
)

set_target_properties(exdir-cpp PROPERTIES
  VERSION "${PROJECT_VERSION}"
  SOVERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}"
  PROJECT_LABEL "exdir-cpp"
)

configure_package_config_file(
  "${PROJECT_SOURCE_DIR}/exdir-cpp-config.cmake.in"
  "${PROJECT_BINARY_DIR}/exdir-cpp-config.cmake"
  INSTALL_DESTINATION "${CMAKE_INSTALL_DATADIR}/cmake/exdir-cpp"
)

write_basic_package_version_file(
  "${PROJECT_BINARY_DIR}/exdir-cpp-config-version.cmake"
  COMPATIBILITY AnyNewerVersion
)

configure_file(exdir-cpp.pc.in exdir-cpp.pc @ONLY)

install(TARGETS exdir-cpp
    EXPORT exdir-cpp-targets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
		FILES_MATCHING PATTERN "*.hpp"
)
  
install(EXPORT exdir-cpp-targets
    DESTINATION "${CMAKE_INSTALL_DATADIR}/cmake/exdir-cpp"
)
	
install(FILES
		"${PROJECT_BINARY_DIR}/exdir-cpp-config.cmake"
		"${PROJECT_BINARY_DIR}/exdir-cpp-config-version.cmake"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/cmake/exdir-cpp"
)
  
install(FILES "${PROJECT_BINARY_DIR}/exdir-cpp.pc"
    DESTINATION ${CMAKE_INSTALL_DATADIR}/pkgconfig
)
