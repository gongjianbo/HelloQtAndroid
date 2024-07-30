#pragma once
#include <QObject>

// 文件操作
class FileTool : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(FileTool)
private:
    FileTool();
public:
    static FileTool *getInstance();
    ~FileTool();

    Q_INVOKABLE int writeTest();
    Q_INVOKABLE int readTest();
};
