cmake_minimum_required(VERSION 3.5)

project(SensorAnalysis VERSION 1.0 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories( app/include )
include_directories( app/src )
include_directories(
    ./external/QtAdvancedDocking/include
)

# Get the GIT hash of the latest commit
include(FindGit OPTIONAL)
if (GIT_FOUND)
        execute_process(
                COMMAND ${GIT_EXECUTABLE} describe --tags --always --dirty=-dev
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                OUTPUT_VARIABLE SOFT_VERSION_GIT
                OUTPUT_STRIP_TRAILING_WHITESPACE
                )
        message(STATUS "building from git ${SOFT_VERSION_GIT}")
endif()

if (NOT SOFT_VERSION_GIT)
        set(SOFT_VERSION_GIT ${PROJECT_VERSION})
endif()

#include(cmake/Common.cmake)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets PrintSupport REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets PrintSupport REQUIRED)
find_package(Qt6 COMPONENTS Network REQUIRED)

#add_subdirectory(./external/protobuf/cmake)

#set(proto_files
# "${CMAKE_CURRENT_LIST_DIR}/proto/Sensor.proto"
#)
#GenerateProtobuf(proto_files outputSim)
#INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
#INCLUDE_DIRECTORIES(./external/protobuf/src)


add_subdirectory( external/QtAdvancedDocking )
add_subdirectory( external/fftw )

if(FFTW3_FOUND)
    include_directories(${FFTW3_INCLUDE_DIRS})
    link_directories(${FFTW3_LIBRARY_DIRS})
else()
    message(FATAL_ERROR "FFTW3 library not found")
endif()

set(INCLUDE_PLOT_LIST
    app/include/qcustomplot.h
    app/include/curvecustomplot.h
    app/include/qcptooltip.h
    app/include/plotcurvecolor.h
    app/include/plotcursor.h
    app/src/qcustomplot.cpp
    app/src/curvecustomplot.cpp
    app/src/qcptooltip.cpp
    app/src/plotcurvecolor.cpp
    app/src/plotcursor.cpp
    app/src/plotfft.cpp
    app/src/mimedata.cpp
    app/src/pluginmanager.cpp
    )

set(INCLUDE_LIST
    app/include/mainwindow.h
    app/include/updatechecker.h
    app/include/socketclient.h
    app/include/menuanimatwidget.h
    app/include/filterwidget.h
    app/include/sortfilterproxymodel.h
    app/include/filterviewwidget.h
    app/include/filtertreeview.h
    app/include/dataloader.h
    app/include/dataloadercsv.h
    app/include/loaderviewwidget.h
    app/include/seriesdataref.h
    app/include/plotwidget.h
    app/include/plotdockerwidget.h
    app/include/plotfft.h
    app/include/mimedata.h
    app/include/pluginInterface.h
    app/include/pluginmanager.h
    )

set(PROJECT_SOURCES
    app_win32.rc
    resource.qrc
    ${INCLUDE_PLOT_LIST}
    ${INCLUDE_LIST}
    app/src/imusetting.ui
    app/src/mainwindow.ui
    app/src/main.cpp
    app/src/mainwindow.cpp 
    app/src/updatechecker.cpp
    app/src/socketclient.cpp
    app/src/menuanimatwidget.cpp
    app/src/filterwidget.cpp
    app/src/sortfilterproxymodel.cpp
    app/src/filterviewwidget.cpp
    app/src/filtertreeview.cpp
    app/src/dataloadercsv.cpp
    app/src/loaderviewwidget.cpp
    app/src/plotwidget.cpp
    app/src/plotdockerwidget.cpp
)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_WIN32_EXECUTABLE TRUE)
endif()

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(SensorAnalysis
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET SensorAnalysis APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(SensorAnalysis SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(SensorAnalysis
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(SensorAnalysis PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::PrintSupport
    PRIVATE Qt6::Network
    qtadvanceddocking
    ${FFTW3_LIBRARIES}
    )

set_target_properties(SensorAnalysis PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(SensorAnalysis)
endif()

configure_file(config.h.cmakein ${CMAKE_CURRENT_BINARY_DIR}/config.h @ONLY)

add_subdirectory(plugins/test)
add_subdirectory(plugins/test2)
