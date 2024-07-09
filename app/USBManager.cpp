#include "USBManager.h"
#include <QtAndroidExtras/QAndroidJniObject>
#include <QtAndroidExtras/QAndroidJniEnvironment>
#include <QtAndroidExtras/QtAndroid>
#include <QMetaObject>
#include <QDebug>
#include "libusb.h"

static void deviceDescriptor(libusb_device *device)
{
    if (!device)
        return;
    libusb_config_descriptor *config;
    libusb_get_config_descriptor(device, 0, &config);
    const libusb_interface *inter;
    const libusb_interface_descriptor *interdesc;
    const libusb_endpoint_descriptor *epdesc;

    libusb_device_descriptor desc;
    int r = libusb_get_device_descriptor(device, &desc);
    if (r < 0) {
        qDebug() << __FUNCTION__ << "Failed to get device descriptor";
        return;
    }
    if (desc.bNumConfigurations > 1) {
        qDebug() << __FUNCTION__ << "Number of possible configurations > 1 !";
    }
    for (int n = 0; n < (int)config->bNumInterfaces; n++)
    {
        inter = &config->interface[n];
        for (int j = 0; j < inter->num_altsetting; j++)
        {
            interdesc = &inter->altsetting[j];
            for (int k = 0; k < (int)interdesc->bNumEndpoints; k++)
            {
                epdesc = &interdesc->endpoint[k];
                qDebug() << n << j << k;
                // 00控制，01同步，02批量，03中断
                qDebug() << "epdesc->bmAttributes" << epdesc->bmAttributes;
                qDebug() << "epdesc->bEndpointAddress" << epdesc->bEndpointAddress;
                qDebug() << "epdesc->bInterval" << epdesc->bInterval;
            }
        }
    }
}

static jboolean openDevice(JNIEnv * /*env*/, jobject /*thiz*/, jint fd, jint vid, jint pid)
{
    bool ret = true;
    QString info = QString("fd: 0x%1, vid: 0x%2, pid: 0x%3")
                       .arg(QString::number(fd, 16))
                       .arg(QString::number(vid, 16))
                       .arg(QString::number(pid, 16));
    qDebug() << __FUNCTION__ << info;
    // 拿到 fd 就可以通过 libusb 的接口进行访问了
    if (ret) {
        QMetaObject::invokeMethod(USBManager::getInstance(), "openDevice", Qt::QueuedConnection,
                                  Q_ARG(int, fd), Q_ARG(int, vid), Q_ARG(int, pid));
    }
    return ret;
}

static void closeDevice(JNIEnv * /*env*/, jobject /*thiz*/)
{
    qDebug() << __FUNCTION__;
    QMetaObject::invokeMethod(USBManager::getInstance(), "closeDevice", Qt::QueuedConnection);
}

static void initUsbManager()
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

USBManager::USBManager()
{
    qDebug() << __FUNCTION__;
    // 用全局对象管理在 libusb_exit 释放时可能卡死，改为构造和析构中调用
    libusb_init(nullptr);
    initUsbManager();
}

USBManager *USBManager::getInstance()
{
    static USBManager instance;
    return &instance;
}

USBManager::~USBManager()
{
    qDebug() << __FUNCTION__;
    doClose();
    libusb_exit(nullptr);
}

bool USBManager::getIsOpen() const
{
    return mIsOpen;
}

QString USBManager::getDeviceInfo() const
{
    return mDeviceInfo;
}

void USBManager::openDevice(int fd, int vid, int pid)
{
    mFd = fd;
    mVid = vid;
    mPid = pid;
    if (!doOpen(fd))
        return;
    mIsOpen = true;
    mDeviceInfo = QString("vid(0x%1) pid(0x%2)").arg(QString::number(vid, 16)).arg(QString::number(pid, 16));
    emit deviceChanged();
}

void USBManager::closeDevice()
{
    doClose();
    mFd = 0;
    mVid = 0;
    mPid = 0;
    mIsOpen = false;
    mDeviceInfo.clear();
    emit deviceChanged();
}

void USBManager::testUsb()
{
    qDebug() << __FUNCTION__;
    if (getIsOpen()) {
        closeDevice();
    }
    openDevice(mFd, mVid, mPid);
}

void USBManager::testUvc()
{
    qDebug() << __FUNCTION__;
    if (getIsOpen()) {
        closeDevice();
    }
    openDevice(mFd, mVid, mPid);
}

bool USBManager::doOpen(int fd)
{
    libusb_device_handle *devh = nullptr;
    int usb_ret = 0;
    usb_ret = libusb_set_option(nullptr, LIBUSB_OPTION_NO_DEVICE_DISCOVERY, nullptr);
    if (usb_ret != LIBUSB_SUCCESS) {
        qDebug() << "libusb_set_option failed:" << usb_ret << libusb_strerror(usb_ret);
        return false;
    }
    usb_ret = libusb_init(&fdCtx);
    if (usb_ret < 0) {
        qDebug() << "libusb_init failed:" << usb_ret << libusb_strerror(usb_ret);
        return false;
    }
    usb_ret = libusb_wrap_sys_device(fdCtx, (intptr_t)fd, &devh);
    if (usb_ret < 0) {
        qDebug() << "libusb_wrap_sys_device failed:" << usb_ret << libusb_strerror(usb_ret);
        return false;
    } else if (!devh) {
        qDebug() << "libusb_wrap_sys_device returned invalid handle";
        return false;
    }
    mHandle = devh;
    mDevice = libusb_get_device(devh);
    deviceDescriptor(mDevice);
    usb_ret = libusb_claim_interface(mHandle, 0);
    if (usb_ret) {
        qDebug() << "libusb_claim_interface error." << usb_ret << libusb_strerror(usb_ret);
        mHandle = nullptr;
        mDevice = nullptr;
        libusb_exit(fdCtx);
        fdCtx = nullptr;
        return false;
    }
    return true;
}

void USBManager::doClose()
{
    if (mHandle) {
        libusb_release_interface(mHandle, 0);
    }
    mDevice = nullptr;
    mHandle = nullptr;
    if (fdCtx) {
        libusb_exit(fdCtx);
        fdCtx = nullptr;
    }
}
