QT += core
QT += gui
QT += widgets
QT += quick
QT += qml
QT += concurrent
QT += network
# Qt5 androidextras
QT += androidextras

CONFIG += utf8_source
CONFIG += c++17

TEMPLATE = app
DESTDIR = $$PWD/bin
ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

SOURCES += \
    main.cpp

RESOURCES += qml.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    android/AndroidManifest.xml \
    android/build.gradle \
    android/gradle.properties \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew \
    android/gradlew.bat \
    android/java/MainActivity.java \
    android/res/values/libs.xml
