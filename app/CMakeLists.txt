cmake_minimum_required(VERSION 3.14)

project("HelloQtAndroid" VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Gui Widgets Quick Qml QuickControls2 Concurrent Svg AndroidExtras)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Widgets Quick Qml QuickControls2 Concurrent Svg AndroidExtras)

include_directories(Declarative)
include_directories(Tools)
set(PROJECT_SOURCES
    Declarative/DeclarativeRegister.h
    Declarative/VideoItem.h
    Declarative/VideoItem.cpp
    Tools/ActivityManager.h
    Tools/ActivityManager.cpp
    Tools/AndroidTool.h
    Tools/AndroidTool.cpp
    Tools/FileTool.h
    Tools/FileTool.cpp
    Tools/ToolsRegister.h
    Tools/USBManager.h
    Tools/USBManager.cpp
    main.cpp
    QML/qml.qrc
)

set(OTHER_FILES
    Platform/android/AndroidManifest.xml
    Platform/android/build.gradle
    Platform/android/gradle.properties
    Platform/android/gradle/wrapper/gradle-wrapper.jar
    Platform/android/gradle/wrapper/gradle-wrapper.properties
    Platform/android/gradlew
    Platform/android/gradlew.bat
    Platform/android/res/values/libs.xml
    Platform/android/java/AndroidTool.java
    Platform/android/java/MyApplication.java
    Platform/android/java/MainActivity.java
    Platform/android/java/USBMonitor.java
)

add_custom_target(${PROJECT_NAME}_Other SOURCES ${OTHER_FILES})

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(${PROJECT_NAME}
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
    set_property(TARGET ${PROJECT_NAME} APPEND PROPERTY
        QT_ANDROID_PACKAGE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Platform/android CACHE INTERNAL "")
else()
    if(ANDROID)
        add_library(${PROJECT_NAME} SHARED ${PROJECT_SOURCES})
        set(ANDROID_PACKAGE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Platform/android CACHE INTERNAL "")
    else()
        add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
    endif()
endif()

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../3rd/libusb/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../3rd/libuvc/include
)

set(LibuvcSoDir ${CMAKE_CURRENT_SOURCE_DIR}/../3rd/libuvc/lib/android/${ANDROID_ABI})

# target_link_directories(${PROJECT_NAME} PRIVATE
#     ${CMAKE_CURRENT_SOURCE_DIR}/../3rd/libuvc/lib/android/${ANDROID_ABI}
# )

set(ANDROID_EXTRA_LIBS
    ${LibuvcSoDir}/libjpeg-turbo1.5.0.so
    ${LibuvcSoDir}/libusb1.0.so
    ${LibuvcSoDir}/libuvc.so
    CACHE INTERNAL "")

target_link_libraries(${PROJECT_NAME}
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Quick
    Qt${QT_VERSION_MAJOR}::Qml
    Qt${QT_VERSION_MAJOR}::QuickControls2
    Qt${QT_VERSION_MAJOR}::Concurrent
    Qt${QT_VERSION_MAJOR}::Svg
    Qt${QT_VERSION_MAJOR}::AndroidExtras
    ${LibuvcSoDir}/libjpeg-turbo1.5.0.so
    ${LibuvcSoDir}/libusb1.0.so
    ${LibuvcSoDir}/libuvc.so
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
    set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.gongjianbo.demo)
endif()
set_target_properties(${PROJECT_NAME} PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_import_qml_plugins(${PROJECT_NAME})
    qt_finalize_executable(${PROJECT_NAME})
endif()
