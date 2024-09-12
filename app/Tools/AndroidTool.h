#pragma once
#include <QObject>

// Android java 工具函数调用
class AndroidTool : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(AndroidTool)
private:
    AndroidTool();
public:
    static AndroidTool *getInstance();
    ~AndroidTool();

    // 重启
    Q_INVOKABLE void restartApp();
    // 返回 Home 界面
    Q_INVOKABLE void gotoHome();
    // 分享单个文件
    Q_INVOKABLE void shareFile(const QString &path,
                               const QString &mimeType = "*/*",
                               const QString &title = "Share");
    // 分享多个文件
    Q_INVOKABLE void shareFiles(const QStringList &paths,
                                const QString &mimeType = "*/*",
                                const QString &title = "Share");
};
