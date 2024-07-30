#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "DeclarativeRegister.h"
#include "ToolsRegister.h"

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

    GongJianBo::Declarative::registerType(&engine);
    GongJianBo::Tools::registerType(&engine);

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
        &app, [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
