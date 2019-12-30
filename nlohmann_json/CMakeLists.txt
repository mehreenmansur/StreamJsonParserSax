add_library(nlohmann_json include/nlohmann/json.hpp)
target_include_directories(nlohmann_json PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(nlohmann_json PROPERTIES LINKER_LANGUAGE CXX)
