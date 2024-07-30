#pragma once
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QTouchEvent>
#include <QtAndroidExtras/QtAndroid>

#include "ActivityManager.h"
#include "AndroidTool.h"
#include "FileTool.h"
#include "USBManager.h"

namespace GongJianBo {

namespace Tools {

#define READ_EXTERNAL_STORAGE "android.permission.READ_EXTERNAL_STORAGE"
#define WRITE_EXTERNAL_STORAGE "android.permission.WRITE_EXTERNAL_STORAGE"
// 动态申请存储权限，但是对于安卓 11 的 ACTION_MANAGE_ALL_FILES_ACCESS_PERMISSION，Qt5 没现成接口
inline void checkPermission()
{
    QtAndroid::PermissionResult r = QtAndroid::checkPermission(WRITE_EXTERNAL_STORAGE);
    if(r == QtAndroid::PermissionResult::Denied)
    {
        QtAndroid::requestPermissionsSync(QStringList() << READ_EXTERNAL_STORAGE << WRITE_EXTERNAL_STORAGE);
        r = QtAndroid::checkPermission(WRITE_EXTERNAL_STORAGE);
        if(r == QtAndroid::PermissionResult::Denied)
        {
            qDebug()<<"requestPermissionsSync failed";
        }
    }
}

inline void registerType(QQmlApplicationEngine *engine)
{
    // QQmlContext *context = engine->rootContext();
    // 可以用全局属性，也可以注册为单例
    // qmlRegisterUncreatableType<USBManager>("Cute.Tools", 1, 0, "USBManager", "USBManager is uncreatable type");
    // engine.rootContext()->setContextProperty("usbManager", USBManager::getInstance());
    qmlRegisterSingletonType<ActivityManager>(
        "GongJianBo.Tools", 1, 0,
        "ActivityManager", [](QQmlEngine *qmlEngine, QJSEngine *){
            qmlEngine->setObjectOwnership(ActivityManager::getInstance(), QQmlEngine::CppOwnership);
            return ActivityManager::getInstance();
        });
    qmlRegisterSingletonType<AndroidTool>(
        "GongJianBo.Tools", 1, 0,
        "AndroidTool", [](QQmlEngine *qmlEngine, QJSEngine *){
            qmlEngine->setObjectOwnership(AndroidTool::getInstance(), QQmlEngine::CppOwnership);
            return AndroidTool::getInstance();
        });
    qmlRegisterSingletonType<FileTool>(
        "GongJianBo.Tools", 1, 0,
        "FileTool", [](QQmlEngine *qmlEngine, QJSEngine *){
            qmlEngine->setObjectOwnership(FileTool::getInstance(), QQmlEngine::CppOwnership);
            return FileTool::getInstance();
        });
    qmlRegisterSingletonType<USBManager>(
        "GongJianBo.Tools", 1, 0,
        "USBManager", [](QQmlEngine *qmlEngine, QJSEngine *){
            qmlEngine->setObjectOwnership(USBManager::getInstance(), QQmlEngine::CppOwnership);
            return USBManager::getInstance();
        });

    QObject::connect(ActivityManager::getInstance(), &ActivityManager::touchEventCanceled,
        engine, [engine]() {
            auto &&root_objects = engine->rootObjects();
            if (root_objects.isEmpty())
                return;
            // 但窗口，其他都用 Dialog/Popup
            QGuiApplication::postEvent(root_objects.first(), new QTouchEvent(QEvent::TouchCancel, 0, 0, 0));
        }, Qt::QueuedConnection);
}

}

}
