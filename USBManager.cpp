#include "USBManager.h"
#include <QtAndroidExtras/QAndroidJniObject>
#include <QtAndroidExtras/QAndroidJniEnvironment>
#include <QtAndroidExtras/QtAndroid>
#include <QMetaObject>
#include <QDebug>

static jboolean openDevice(JNIEnv * /*env*/, jobject /*thiz*/, jint fd, jint vid, jint pid)
{
    bool ret = true;
    qDebug() << __FUNCTION__ << fd << vid << pid;
    // 拿到 fd 就可以通过 libusb 的接口进行访问了
    if (ret) {
        // 如果是设备枚举出来的名字，java USBDevice 就可以获取，也可能是设备打开后读取到的标识
        QString name = QString("%1 - %2").arg(vid).arg(pid);
        QMetaObject::invokeMethod(USBManager::getInstance(), "setDevice", Qt::QueuedConnection,
                                  Q_ARG(bool, ret), Q_ARG(QString, name));
    }
    return ret;
}

static void closeDevice(JNIEnv * /*env*/, jobject /*thiz*/)
{
    qDebug() << __FUNCTION__;
    QMetaObject::invokeMethod(USBManager::getInstance(), "setDevice", Qt::QueuedConnection,
                              Q_ARG(bool, false), Q_ARG(QString, ""));
}

USBManager::USBManager()
{
    qDebug() << __FUNCTION__;
    jniRegister();
}

USBManager *USBManager::getInstance()
{
    static USBManager instance;
    return &instance;
}

USBManager::~USBManager()
{
    qDebug() << __FUNCTION__;
}

bool USBManager::getIsOpen() const
{
    return mIsOpen;
}

QString USBManager::getDeviceName() const
{
    return mDeviceName;
}

void USBManager::jniRegister()
{
    JNINativeMethod methods[] =
        {{"openDevice", "(III)Z", reinterpret_cast<void *>(openDevice)},
         {"closeDevice", "()V", reinterpret_cast<void *>(closeDevice)}};

    // 通过自定义的 Application 获取 context，也可以通过当前 activity 获取
    QAndroidJniObject context = QAndroidJniObject::callStaticObjectMethod(
        "com/gongjianbo/demo/MyApplication",
        "getContext",
        "()Landroid/content/Context;");
    if (!context.isValid()) return;

    // 如果用无参构造，直接写 QAndroidJniObject("com/gongjianbo/demo/USBMonitor");
    // 调用时要注意 Qt 的 UI 线程不是安卓的 UI 线程
    QAndroidJniObject monitor(
        "com/gongjianbo/demo/USBMonitor",
        "(Landroid/content/Context;)V",
        context.object<jobject>());

    if (!monitor.isValid()) return;

    QAndroidJniEnvironment env;
    jclass obj_class = env->GetObjectClass(monitor.object<jobject>());
    if (!obj_class) return;

    env->RegisterNatives(obj_class,
                         methods,
                         sizeof(methods) / sizeof(methods[0]));
    env->DeleteLocalRef(obj_class);
}

void USBManager::setDevice(const bool isOpen, const QString &deviceName)
{
    if (isOpen == mIsOpen && deviceName == mDeviceName)
        return;
    mIsOpen = isOpen;
    mDeviceName = deviceName;
    emit deviceChanged();
}
