QT += core
QT += gui
QT += widgets
QT += quick
QT += qml
QT += quickcontrols2
QT += concurrent
QT += svg

CONFIG += utf8_source
CONFIG += c++17

TEMPLATE = app
DESTDIR = $$PWD/../bin

SOURCES += main.cpp

RESOURCES += QML/qml.qrc

INCLUDEPATH += $$PWD/Tools
include($$PWD/Tools/Tools.pri)

INCLUDEPATH += $$PWD/Declarative
include($$PWD/Declarative/Declarative.pri)

# INCLUDEPATH += $$PWD/../lib
# DEPENDPATH += $$PWD/../lib
# LIBS += -L$$PWD/../bin/ -llib_arm64-v8a

android{
    # Qt5 androidextras
    QT += androidextras
    ANDROID_PACKAGE_SOURCE_DIR = $$PWD/Platform/android

    DISTFILES += \
        Platform/android/AndroidManifest.xml \
        Platform/android/build.gradle \
        Platform/android/gradle.properties \
        Platform/android/gradle/wrapper/gradle-wrapper.jar \
        Platform/android/gradle/wrapper/gradle-wrapper.properties \
        Platform/android/gradlew \
        Platform/android/gradlew.bat \
        Platform/android/res/values/libs.xml

    DISTFILES += \
        Platform/android/java/AndroidTool.java \
        Platform/android/java/MyApplication.java \
        Platform/android/java/MainActivity.java \
        Platform/android/java/USBMonitor.java

    # ANDROID_ABIS = arm64-v8a armeabi-v7a x86 x86_64
    INCLUDEPATH += $$PWD/../3rd/libusb/include
    INCLUDEPATH += $$PWD/../3rd/libuvc/include
    # LIBS += $$PWD/../3rd/libusb/lib/android/$${ANDROID_TARGET_ARCH}/libusb1.0.so
    LIBS += $$PWD/../3rd/libuvc/lib/android/$${ANDROID_TARGET_ARCH}/libuvc.so
    LIBS += $$PWD/../3rd/libuvc/lib/android/$${ANDROID_TARGET_ARCH}/libusb1.0.so
    LIBS += $$PWD/../3rd/libuvc/lib/android/$${ANDROID_TARGET_ARCH}/libjpeg-turbo1.5.0.so
    for(abi, ANDROID_ABIS){
        # ANDROID_EXTRA_LIBS += $$PWD/../3rd/libusb/lib/android/$${abi}/libusb1.0.so
        ANDROID_EXTRA_LIBS += $$PWD/../3rd/libuvc/lib/android/$${abi}/libuvc.so
        ANDROID_EXTRA_LIBS += $$PWD/../3rd/libuvc/lib/android/$${abi}/libusb1.0.so
        ANDROID_EXTRA_LIBS += $$PWD/../3rd/libuvc/lib/android/$${abi}/libjpeg-turbo1.5.0.so
    }
}

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
