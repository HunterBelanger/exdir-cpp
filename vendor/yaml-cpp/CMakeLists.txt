cmake_minimum_required(VERSION 3.1)
project(yaml-cpp LANGUAGES CXX VERSION 0.6.3)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

set(YAML_CPP_SOURCE_FILES ${YAML_CPP_SOURCE_FILES}
	src/binary.cpp
	src/emit.cpp
	src/emitterstate.cpp
	src/exp.cpp
	src/node.cpp
	src/null.cpp
	src/parser.cpp
	src/scanscalar.cpp
	src/simplekey.cpp
	src/tag.cpp
	src/convert.cpp
	src/emitfromevents.cpp
	src/emitterutils.cpp
	src/memory.cpp
	src/node_data.cpp
	src/ostream_wrapper.cpp
	src/regex_yaml.cpp
	src/scantag.cpp
	src/singledocparser.cpp
	src/directives.cpp
	src/emitter.cpp
	src/exceptions.cpp
	src/nodebuilder.cpp
	src/nodeevents.cpp
	src/parse.cpp
	src/scanner.cpp
	src/scantoken.cpp
	src/stream.cpp	
)

add_library(yaml-cpp OBJECT ${YAML_CPP_SOURCE_FILES})
set_target_properties(yaml-cpp PROPERTIES PUBLIC_HEADER "${YAML_CPP_HEADER_FILES}")

set_property(TARGET yaml-cpp PROPERTY POSITION_INDEPENDENT_CODE 1)

add_library(yaml-cpp_shared SHARED $<TARGET_OBJECTS:yaml-cpp>)
add_library(yaml-cpp_static STATIC $<TARGET_OBJECTS:yaml-cpp>)

set_target_properties(yaml-cpp_shared PROPERTIES OUTPUT_NAME yaml-cpp)
set_target_properties(yaml-cpp_static PROPERTIES OUTPUT_NAME yaml-cpp)

install(TARGETS yaml-cpp_shared yaml-cpp_static
				LIBRARY
					DESTINATION lib
					COMPONENT Libraries
					NAMELINK_COMPONENT Development
				PUBLIC_HEADER
					DESTINATION include
					COMPONENT Development
)

install(DIRECTORY "include/yaml-cpp" DESTINATION include)