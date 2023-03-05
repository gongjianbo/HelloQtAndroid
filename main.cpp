#include <QApplication>
#include <QQmlApplicationEngine>

#ifdef Q_OS_ANDROID
#include <QtAndroidExtras/QtAndroid>
#define READ_EXTERNAL_STORAGE "android.permission.READ_EXTERNAL_STORAGE"
#define WRITE_EXTERNAL_STORAGE "android.permission.WRITE_EXTERNAL_STORAGE"
// 授权检测
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
#endif

int main(int argc, char *argv[])
{
#ifdef Q_OS_ANDROID
    checkPermission();
#endif
    qputenv("QML2_IMPORT_PATH", "");
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QCoreApplication::setOrganizationName("gongjianbo");
    QCoreApplication::setOrganizationDomain("gongjianbo.org");
    QCoreApplication::setApplicationName("HelloQtAndroid");
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
