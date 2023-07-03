QT += core
QT += gui
QT += widgets
QT += quick
QT += qml
QT += concurrent

CONFIG += utf8_source
CONFIG += c++17

TEMPLATE = app

HEADERS += \
    USBManager.h

SOURCES += \
    USBManager.cpp \
    main.cpp

DESTDIR = $$PWD/bin
RESOURCES += qml.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

android{
    # Qt5 androidextras
    QT += androidextras
    ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

    DISTFILES += \
        android/AndroidManifest.xml \
        android/build.gradle \
        android/gradle.properties \
        android/gradle/wrapper/gradle-wrapper.jar \
        android/gradle/wrapper/gradle-wrapper.properties \
        android/gradlew \
        android/gradlew.bat \
        android/res/values/libs.xml

    DISTFILES += \
        android/java/MyApplication.java \
        android/java/MainActivity.java \
        android/java/USBMonitor.java

    # LIBS +=

    # ANDROID_EXTRA_LIBS +=
}
