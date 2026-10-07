find_package(Qt6 REQUIRED COMPONENTS Widgets OpenGLWidgets)

qt_standard_project_setup()

qt_add_executable(viewer main.cpp)

target_link_libraries(viewer PRIVATE core Qt6::Widgets Qt6::OpenGLWidgets)

set_project_warnings(viewer)