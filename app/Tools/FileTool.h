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

    // 文件读写测试
    Q_INVOKABLE int writeTest();
    Q_INVOKABLE int readTest();
    // 文件分享测试
    Q_INVOKABLE void shareFileTest();
    Q_INVOKABLE void shareFilesTest();
};
