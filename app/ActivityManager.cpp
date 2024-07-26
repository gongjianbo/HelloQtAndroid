#include "ActivityManager.h"
#include <QtAndroidExtras/QAndroidJniObject>
#include <QtAndroidExtras/QAndroidJniEnvironment>
#include <QtAndroidExtras/QtAndroid>
#include <QDebug>

void Java_com_gongjianbo_demo_MainActivity_jniTouchEventCanceled(JNIEnv * /*env*/, jobject /*thiz*/)
{
    qDebug() << __FUNCTION__;
    emit ActivityManager::getInstance()->touchEventCanceled();
}

ActivityManager::ActivityManager()
{
    qDebug() << __FUNCTION__;
}

ActivityManager *ActivityManager::getInstance()
{
    static ActivityManager instance;
    return &instance;
}

ActivityManager::~ActivityManager()
{
    qDebug() << __FUNCTION__;
}
