#include "AndroidTool.h"
#include <QtAndroidExtras/QAndroidJniObject>
#include <QtAndroidExtras/QAndroidJniEnvironment>
#include <QtAndroidExtras/QtAndroid>
#include <QDebug>

AndroidTool::AndroidTool()
{
    qDebug() << __FUNCTION__;
}

AndroidTool *AndroidTool::getInstance()
{
    static AndroidTool instance;
    return &instance;
}

AndroidTool::~AndroidTool()
{
    qDebug() << __FUNCTION__;
}

void AndroidTool::restartApp()
{
    QAndroidJniObject::callStaticMethod<void>(
        "com/gongjianbo/demo/AndroidTool",
        "restartApp",
        "()V");
}

void AndroidTool::gotoHome()
{
    QAndroidJniObject::callStaticMethod<void>(
        "com/gongjianbo/demo/AndroidTool",
        "gotoHome",
        "()V");
}
