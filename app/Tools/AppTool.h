#pragma once
#include <QObject>
#include <QVariant>
#include <QUrl>
#include <QImage>

// 通用工具函数
class AppTool : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(AppTool)
private:
    AppTool();
public:
    static AppTool *getInstance();
    ~AppTool();

    // 内部存储路径
    Q_INVOKABLE QString appDataPath();
    // 外部存储路径
    Q_INVOKABLE QString genericDataPath();
    // 获取grabToImage回调(ItemGrabResult)result.image的信息
    Q_INVOKABLE void imageInfo(const QVariant &var);
};
