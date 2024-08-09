#include "ActivityManager.h"
#include <QtAndroidExtras/QAndroidJniObject>
#include <QtAndroidExtras/QAndroidJniEnvironment>
#include <QtAndroidExtras/QtAndroid>
#include <QMetaObject>
#include <QDebug>

void jniTouchEventCanceled(JNIEnv * /*env*/, jobject /*thiz*/)
{
    qDebug() << __FUNCTION__;
    emit ActivityManager::getInstance()->touchEventCanceled();
}

void jniActivityRotationChanged(JNIEnv * /*env*/, jobject /*thiz*/, jint rotation)
{
    qDebug() << __FUNCTION__ << rotation;
    QMetaObject::invokeMethod(ActivityManager::getInstance(), [rotation](){
        ActivityManager::getInstance()->setRotation(rotation);
    }, Qt::QueuedConnection);
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
        {{"jniTouchEventCanceled", "()V", reinterpret_cast<void *>(jniTouchEventCanceled)},
         {"jniActivityRotationChanged", "(I)V", reinterpret_cast<void *>(jniActivityRotationChanged)}};

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
    // 初始化角度
    activity.callMethod<void>("initRotation", "()V");
}

int ActivityManager::getRotation() const
{
    return mRotation;
}

void ActivityManager::setRotation(int rotation)
{
    if (mRotation == rotation)
        return;
    mRotation = rotation;
    emit rotationChanged();
}

void ActivityManager::enableRotationListen()
{
    QAndroidJniObject activity = QtAndroid::androidActivity();
    if (!activity.isValid())
        return;
    activity.callMethod<void>("enableRotationListen", "()V");
}

void ActivityManager::disableRotationListen()
{
    QAndroidJniObject activity = QtAndroid::androidActivity();
    if (!activity.isValid())
        return;
    activity.callMethod<void>("disableRotationListen", "()V");
}
