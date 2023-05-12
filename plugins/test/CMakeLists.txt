cmake_minimum_required(VERSION 3.5)

project(test VERSION 1.0 LANGUAGES CXX)

add_definitions(-DPLUGIN01_LIBRARY)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories( include )
include_directories( src )

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets PrintSupport REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets PrintSupport REQUIRED)
find_package(Qt6 COMPONENTS Network REQUIRED)

#add_subdirectory( test )

set(INCLUDE_LIST
    include/plugin01.h
    include/Plugin01_global.h
    include/PluginInterface.h
    )

set(PROJECT_SOURCES
    ${INCLUDE_LIST}
    src/plugin01.cpp
)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_WIN32_EXECUTABLE TRUE)
endif()

add_library(test SHARED
    ${PROJECT_SOURCES}
)

target_link_libraries(test PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::PrintSupport
    PRIVATE Qt6::Network
    )

set_target_properties(test PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my_1.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

#if(QT_VERSION_MAJOR EQUAL 6)
#    qt_finalize_executable(test)
#endif()

#configure_file(config.h.cmakein ${CMAKE_CURRENT_BINARY_DIR}/config.h @ONLY)
