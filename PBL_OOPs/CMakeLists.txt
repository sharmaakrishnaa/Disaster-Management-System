
cmake_minimum_required(VERSION 3.16)

project(PBL_OOPs VERSION 0.1 LANGUAGES CXX)

# Enable Qt automatic processing
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Use C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt Widgets (supports Qt 5 and Qt 6)
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# Current project source files
set(PROJECT_SOURCES
    main.cpp

    mainwindow.cpp
    mainwindow.h
    mainwindow.ui

    registerwindow.cpp
    registerwindow.h

    rolewindow.cpp
    rolewindow.h

    victimdashboard.cpp
    victimdashboard.h

    volunteerdashboard.cpp
    volunteerdashboard.h

    ngodashboard.cpp
    ngodashboard.h

    admindashboard.cpp
    admindashboard.h

)

# Create executable
if(QT_VERSION_MAJOR GREATER_EQUAL 6)
    qt_add_executable(PBL_OOPs
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        AppTypes.h
        sosrequest.h sosrequest.cpp
        sosrequestservice.h sosrequestservice.cpp
    )
else()
    if(ANDROID)
        add_library(PBL_OOPs SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(PBL_OOPs
            ${PROJECT_SOURCES}
        )
    endif()
endif()

# Link Qt Widgets
target_link_libraries(PBL_OOPs
    PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
)

# Platform properties
if(QT_VERSION_MAJOR EQUAL 6)
    set_target_properties(PBL_OOPs PROPERTIES
        MACOSX_BUNDLE_GUI_IDENTIFIER com.example.PBL_OOPs
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        MACOSX_BUNDLE TRUE
        WIN32_EXECUTABLE TRUE
    )
else()
    set_target_properties(PBL_OOPs PROPERTIES
        MACOSX_BUNDLE_GUI_IDENTIFIER com.example.PBL_OOPs
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        MACOSX_BUNDLE TRUE
        WIN32_EXECUTABLE TRUE
    )
endif()

# Installation directories
include(GNUInstallDirs)

install(TARGETS PBL_OOPs
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Finalize Qt 6 executable
if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(PBL_OOPs)
endif()
