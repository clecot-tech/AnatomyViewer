add_library(core STATIC version.cpp)
target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

set_project_warnings(core)