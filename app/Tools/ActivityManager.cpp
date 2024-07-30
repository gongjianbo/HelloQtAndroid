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

void jniTouchEventCanceled(JNIEnv * /*env*/, jobject /*thiz*/)
{
    qDebug() << __FUNCTION__;
    emit ActivityManager::getInstance()->touchEventCanceled();
}

// 动态注册 JNI 接口
void initActivityManager()
{
    JNINativeMethod methods[] =
        {{"jniTouchEventCanceled", "()V", reinterpret_cast<void *>(jniTouchEventCanceled)}};

    QAndroidJniObject activity = QtAndroid::androidActivity();
    if (!activity.isValid()) return;

    QAndroidJniEnvironment env;
    jclass obj_class = env->GetObjectClass(activity.object<jobject>());
    if (!obj_class) return;

    env->RegisterNatives(obj_class,
                         methods,
                         sizeof(methods) / sizeof(methods[0]));
    env->DeleteLocalRef(obj_class);
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
