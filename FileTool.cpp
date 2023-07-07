#include "FileTool.h"
#include <QStandardPaths>
#include <QFile>
#include <QDir>

FileTool::FileTool(QObject *parent)
    : QObject{parent}
{

}

int FileTool::writeTest()
{
    QString dirpath = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation)
        + "/HelloQtAndroid";
    QDir dir(dirpath);
    if (!dir.exists() && !dir.mkpath(dirpath))
        return -999;
    QString filepath = dirpath + "/test.txt";
    QFile file(filepath);
    int ret = 0;
    if (file.open(QIODevice::WriteOnly)) {
        ret = file.write("hello world!");
        file.close();
    }
    return ret;
}

int FileTool::readTest()
{
    QString dirpath = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation)
                      + "/HelloQtAndroid";
    QDir dir(dirpath);
    if (!dir.exists())
        return -999;
    QString filepath = dirpath + "/test.txt";
    QFile file(filepath);
    int ret = 0;
    if (file.open(QIODevice::ReadOnly)) {
        ret = file.readAll().size();
        file.close();
    }
    return ret;
}
