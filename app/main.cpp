#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "ActivityManager.h"
#include "USBManager.h"
#include "FileTool.h"
#include "VideoItem.h"

#include <QtAndroidExtras/QtAndroid>
#define READ_EXTERNAL_STORAGE "android.permission.READ_EXTERNAL_STORAGE"
#define WRITE_EXTERNAL_STORAGE "android.permission.WRITE_EXTERNAL_STORAGE"
// 动态申请存储权限，但是对于安卓 11 的 ACTION_MANAGE_ALL_FILES_ACCESS_PERMISSION，Qt5 没现成接口
void checkPermission()
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

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QCoreApplication::setOrganizationName("gongjianbo");
    QCoreApplication::setOrganizationDomain("gongjianbo.com");
    QCoreApplication::setApplicationName("HelloQtAndroid");
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    qRegisterMetaType<QImage>("QImage");

    qmlRegisterType<FileTool>("GongJianBo", 1, 0, "FileTool");
    qmlRegisterType<VideoItem>("GongJianBo", 1, 0, "VideoItem");
    // qmlRegisterUncreatableType<USBManager>("GongJianBo", 1, 0, "USBManager", "USBManager is uncreatable type");
    // 可以用全局属性，也可以注册为单例
    // engine.rootContext()->setContextProperty("usbManager", USBManager::getInstance());
    qmlRegisterSingletonType<ActivityManager>(
        "GongJianBo", 1, 0,
        "ActivityManager", [](QQmlEngine *qmlEngine, QJSEngine *){
            qmlEngine->setObjectOwnership(ActivityManager::getInstance(), QQmlEngine::CppOwnership);
            return ActivityManager::getInstance();
        });
    qmlRegisterSingletonType<USBManager>(
        "GongJianBo", 1, 0,
        "USBManager", [](QQmlEngine *qmlEngine, QJSEngine *){
            qmlEngine->setObjectOwnership(USBManager::getInstance(), QQmlEngine::CppOwnership);
            return USBManager::getInstance();
        });

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
        &app, [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        }, Qt::QueuedConnection);
    engine.load(url);

    QObject::connect(ActivityManager::getInstance(), &ActivityManager::touchEventCanceled,
        &engine, [&engine]() {
            if (engine.rootObjects().isEmpty())
                return;
            // qDebug() << engine.rootObjects() << engine.rootObjects().first()->children();
            QGuiApplication::postEvent(engine.rootObjects().first(), new QTouchEvent( QEvent::TouchCancel, 0, 0, 0));
        }, Qt::QueuedConnection);

    return app.exec();
}
