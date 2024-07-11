#pragma once
#include <QObject>

// 文件操作
class FileTool : public QObject
{
    Q_OBJECT
public:
    explicit FileTool(QObject *parent = nullptr);

    Q_INVOKABLE int writeTest();
    Q_INVOKABLE int readTest();

};
