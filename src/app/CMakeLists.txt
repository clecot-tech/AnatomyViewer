find_package(Qt6 REQUIRED COMPONENTS Widgets OpenGLWidgets)

qt_standard_project_setup()

qt_add_executable(viewer main.cpp)

target_link_libraries(viewer PRIVATE core Qt6::Widgets Qt6::OpenGLWidgets)

set_project_warnings(viewer)

if(WIN32)
    add_custom_command(TARGET viewer POST_BUILD
        COMMAND Qt6::windeployqt "$<TARGET_FILE:viewer>"
        COMMENT "Copie des DLL Qt")
endif()