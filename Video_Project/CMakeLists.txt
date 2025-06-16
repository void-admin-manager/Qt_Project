cmake_minimum_required(VERSION 3.16)

project(Video_Project VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


find_package(Qt6 COMPONENTS Widgets Multimedia MultimediaWidgets REQUIRED)

set(PROJECT_SOURCES
    main.cpp
    widget.cpp
    widget.h
    widget.ui
)

if(Qt6_VERSION)
    qt_add_executable(Video_Project
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        videoslider.h videoslider.cpp
        resource.qrc
    )
else()
    message(FATAL_ERROR "Qt6 is required")
endif()

target_link_libraries(Video_Project PRIVATE
    Qt6::Widgets
    Qt6::Multimedia
    Qt6::MultimediaWidgets
)

if(APPLE)
    if(Qt6_VERSION VERSION_LESS 6.1.0)
        set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.Video_Project)
    endif()

    set_target_properties(Video_Project PROPERTIES
        ${BUNDLE_ID_OPTION}
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        MACOSX_BUNDLE TRUE
    )
endif()

if(WIN32)
    set_target_properties(Video_Project PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

include(GNUInstallDirs)
install(TARGETS Video_Project
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(TARGET Video_Project AND Qt6_VERSION)
    qt_finalize_executable(Video_Project)
endif()
