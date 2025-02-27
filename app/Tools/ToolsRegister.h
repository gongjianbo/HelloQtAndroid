#pragma once
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QTouchEvent>
#include <QtAndroidExtras/QtAndroid>

#include "ActivityManager.h"
#include "AndroidTool.h"
#include "AppTool.h"
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
    ActivityManager::getInstance()->initJNI();
    USBManager::getInstance()->initJNI();

    // QQmlContext *context = engine->rootContext();
    // 可以用全局属性，也可以注册为单例
    // qmlRegisterUncreatableType<USBManager>("Cute.Tools", 1, 0, "USBManager", "USBManager is uncreatable type");
    // engine.rootContext()->setContextProperty("usbManager", USBManager::getInstance());

    engine->setObjectOwnership(ActivityManager::getInstance(), QQmlEngine::CppOwnership);
    qmlRegisterSingletonType<ActivityManager>(
        "GongJianBo.Tools", 1, 0,
        "ActivityManager", [](QQmlEngine *, QJSEngine *){
            return ActivityManager::getInstance();
        });
    engine->setObjectOwnership(AndroidTool::getInstance(), QQmlEngine::CppOwnership);
    qmlRegisterSingletonType<AndroidTool>(
        "GongJianBo.Tools", 1, 0,
        "AndroidTool", [](QQmlEngine *, QJSEngine *){
            return AndroidTool::getInstance();
        });
    engine->setObjectOwnership(AppTool::getInstance(), QQmlEngine::CppOwnership);
    qmlRegisterSingletonType<AppTool>(
        "GongJianBo.Tools", 1, 0,
        "AppTool", [](QQmlEngine *, QJSEngine *){
            return AppTool::getInstance();
        });
    engine->setObjectOwnership(FileTool::getInstance(), QQmlEngine::CppOwnership);
    qmlRegisterSingletonType<FileTool>(
        "GongJianBo.Tools", 1, 0,
        "FileTool", [](QQmlEngine *, QJSEngine *){
            return FileTool::getInstance();
        });
    engine->setObjectOwnership(USBManager::getInstance(), QQmlEngine::CppOwnership);
    qmlRegisterSingletonType<USBManager>(
        "GongJianBo.Tools", 1, 0,
        "USBManager", [](QQmlEngine *, QJSEngine *){
            return USBManager::getInstance();
        });

    QObject::connect(ActivityManager::getInstance(), &ActivityManager::touchEventCanceled,
                     engine, [engine]() {
                         auto &&root_objects = engine->rootObjects();
                         if (root_objects.isEmpty())
                             return;
                         // 单窗口，其他都用 Dialog/Popup
                         QGuiApplication::postEvent(root_objects.first(), new QTouchEvent(QEvent::TouchCancel));
                     }, Qt::QueuedConnection);
}

}

}
