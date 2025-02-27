#include "AppTool.h"
#include <QStandardPaths>
#include <QDebug>

AppTool::AppTool()
{

}

AppTool *AppTool::getInstance()
{
    static AppTool instance;
    return &instance;
}

AppTool::~AppTool()
{

}

QString AppTool::appDataPath()
{
    return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
}

QString AppTool::genericDataPath()
{
    return QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
}

void AppTool::imageInfo(const QVariant &var)
{
    QImage image = var.value<QImage>();
    qDebug() << image;
}
