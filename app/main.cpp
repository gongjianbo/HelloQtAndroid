#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "USBManager.h"
#include "FileTool.h"

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
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    qmlRegisterType<FileTool>("GongJianBo", 1, 0, "FileTool");
    engine.rootContext()->setContextProperty("usbManager", USBManager::getInstance());

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
