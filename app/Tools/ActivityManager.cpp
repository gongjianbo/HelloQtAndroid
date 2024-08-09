#include "ActivityManager.h"
#include <QtAndroidExtras/QAndroidJniObject>
#include <QtAndroidExtras/QAndroidJniEnvironment>
#include <QtAndroidExtras/QtAndroid>
#include <QDebug>

void jniTouchEventCanceled(JNIEnv * /*env*/, jobject /*thiz*/)
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

void ActivityManager::initJNI()
{
    JNINativeMethod methods[] =
        {{"jniTouchEventCanceled", "()V", reinterpret_cast<void *>(jniTouchEventCanceled)}};

    QAndroidJniObject activity = QtAndroid::androidActivity();
    if (!activity.isValid()) return;

    QAndroidJniEnvironment env;
    jclass obj_class = env->GetObjectClass(activity.object<jobject>());
    if (!obj_class) return;
    // RegisterNatives 动态注册 JNI 接口
    env->RegisterNatives(obj_class,
                         methods,
                         sizeof(methods) / sizeof(methods[0]));
    env->DeleteLocalRef(obj_class);
}
