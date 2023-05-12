cmake_minimum_required(VERSION 3.5)
project(QtAdvancedDockingSystem LANGUAGES CXX)
find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} 5.5 COMPONENTS Core Gui Widgets REQUIRED)
if (UNIX AND NOT APPLE)
    include_directories(${Qt${QT_VERSION_MAJOR}Gui_PRIVATE_INCLUDE_DIRS})
endif()
set(CMAKE_INCLUDE_CURRENT_DIR ON)
if(BUILD_STATIC)
    set(CMAKE_STATIC_LIBRARY_SUFFIX "_static${CMAKE_STATIC_LIBRARY_SUFFIX}")
endif()
set(ads_SRCS
    src/ads_globals.cpp
    src/DockAreaTabBar.cpp
    src/DockAreaTitleBar.cpp
    src/DockAreaWidget.cpp
    src/DockContainerWidget.cpp
    src/DockManager.cpp
    src/DockOverlay.cpp
    src/DockSplitter.cpp
    src/DockWidget.cpp
    src/DockWidgetTab.cpp
    src/DockingStateReader.cpp
    src/DockFocusController.cpp
    src/ElidingLabel.cpp
    src/FloatingDockContainer.cpp
    src/FloatingDragPreview.cpp
    src/IconProvider.cpp
    src/DockComponentsFactory.cpp
    src/ads.qrc
)
set(ads_HEADERS
    include/ads_globals.h
    include/DockAreaTabBar.h
    include/DockAreaTitleBar.h
    include/DockAreaTitleBar_p.h
    include/DockAreaWidget.h
    include/DockContainerWidget.h
    include/DockManager.h
    include/DockOverlay.h
    include/DockSplitter.h
    include/DockWidget.h
    include/DockWidgetTab.h
    include/DockingStateReader.h
    include/DockFocusController.h
    include/ElidingLabel.h
    include/FloatingDockContainer.h
    include/FloatingDragPreview.h
    include/IconProvider.h
    include/DockComponentsFactory.h
)
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
if (UNIX AND NOT APPLE)
    set(ads_SRCS linux/FloatingWidgetTitleBar.cpp ${ads_SRCS})
    set(ads_HEADERS linux/FloatingWidgetTitleBar.h ${ads_HEADERS})
endif()
if(BUILD_STATIC)
    add_library(qtadvanceddocking STATIC ${ads_SRCS} ${ads_HEADERS})
    target_compile_definitions(qtadvanceddocking PUBLIC ADS_STATIC)
else()
    add_library(qtadvanceddocking SHARED ${ads_SRCS} ${ads_HEADERS})
    target_compile_definitions(qtadvanceddocking PRIVATE ADS_SHARED_EXPORT)
endif()
target_link_libraries(qtadvanceddocking PUBLIC Qt${QT_VERSION_MAJOR}::Core 
                                               Qt${QT_VERSION_MAJOR}::Gui 
                                               Qt${QT_VERSION_MAJOR}::Widgets)
set_target_properties(qtadvanceddocking PROPERTIES
    AUTOMOC ON
    AUTORCC ON
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)
