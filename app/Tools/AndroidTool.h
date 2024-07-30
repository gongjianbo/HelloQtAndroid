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
};
