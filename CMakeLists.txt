cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(JSON_BuildTests OFF CACHE INTERNAL "")

project(stream_json_parser_sax)

add_subdirectory(nlohmann_json)
add_executable(stream_json_parser_sax stream_json_parser_sax.cpp)

target_include_directories(stream_json_parser_sax
       	PUBLIC 
	"${PROJECT_BINARY_DIR}"
	"${PROJECT_SOURCE_DIR}/nlohmann_json/include"
	)

configure_file(data.json data.json COPYONLY)
