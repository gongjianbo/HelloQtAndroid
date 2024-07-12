#include "USBManager.h"
#include <QtAndroidExtras/QAndroidJniObject>
#include <QtAndroidExtras/QAndroidJniEnvironment>
#include <QtAndroidExtras/QtAndroid>
#include <QMetaObject>
#include <QTime>
#include <QDebug>
// dup(fd)
#include "unistd.h"

void deviceDescriptor(libusb_device *device)
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

jboolean jniDeviceAttach(JNIEnv * env, jobject /*thiz*/, jint fd, jint vid, jint pid, jstring deviceName, jstring productName)
{
    bool ret = true;
    QString device_name = env->GetStringUTFChars(deviceName, 0);
    QString product_name = env->GetStringUTFChars(productName, 0);
    QString info = QString("fd: 0x%1, vid: 0x%2, pid: 0x%3, device: %4, product: %5")
                       .arg(QString::number(fd, 16))
                       .arg(QString::number(vid, 16))
                       .arg(QString::number(pid, 16))
                       .arg(device_name)
                       .arg(product_name);
    qDebug() << __FUNCTION__ << info;
    if (ret) {
        QMetaObject::invokeMethod(USBManager::getInstance(), "onDeviceAttach", Qt::QueuedConnection,
                                  Q_ARG(int, fd), Q_ARG(int, vid), Q_ARG(int, pid),
                                  Q_ARG(QString, device_name), Q_ARG(QString, product_name));
    }
    return ret;
}

void jniDeviceDetach(JNIEnv * /*env*/, jobject /*thiz*/)
{
    qDebug() << __FUNCTION__;
    QMetaObject::invokeMethod(USBManager::getInstance(), "onDeviceDetach", Qt::QueuedConnection);
}

void initUsbManager()
{
    JNINativeMethod methods[] =
        {{"jniDeviceAttach", "(IIILjava/lang/String;Ljava/lang/String;)Z", reinterpret_cast<void *>(jniDeviceAttach)},
         {"jniDeviceDetach", "()V", reinterpret_cast<void *>(jniDeviceDetach)}};

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
    // libusb_init(nullptr);
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
    testClose();
    // libusb_exit(nullptr);
}

QString USBManager::getDeviceInfo() const
{
    return mDeviceInfo;
}

int USBManager::getDeviceMode() const
{
    return mDeviceMode;
}

bool USBManager::getIsOpen() const
{
    return mIsOpen;
}

void USBManager::onDeviceAttach(int fd, int vid, int pid, const QString &deviceName, const QString &productName)
{
    mFd = dup(fd);
    mVid = vid;
    mPid = pid;
    mBusNum = 0;
    mDevAddr = 0;
    mUsbFs = QByteArray();
    const QStringList device_split = deviceName.split('/');
    const int len = device_split.size();
    if (len > 2) {
        mBusNum = device_split.at(len - 2).toInt();
        mDevAddr = device_split.at(len - 1).toInt();
        for (int i = 1; i < len - 2; i++) {
            mUsbFs.append("/");
            mUsbFs.append(device_split.at(i).toLatin1());
        }
    }
    if (mUsbFs.isEmpty()) {
        mUsbFs = "/dev/bus/usb";
    }
    mDeviceInfo = QString("vid(0x%1) pid(0x%2)").arg(QString::number(vid, 16)).arg(QString::number(pid, 16));
    qDebug() << __FUNCTION__ << fd << vid << pid << deviceName << productName;
    qDebug() << mDeviceInfo << mBusNum << mDevAddr << mUsbFs;
    emit deviceInfoChanged();
    testOpen();
}

void USBManager::onDeviceDetach()
{
    testClose();
    close(mFd);
    mDeviceInfo = QString();
    emit deviceInfoChanged();
}

void USBManager::testOpen(int mode)
{
    testClose();
    if (mode != TestNone) {
        mDeviceMode = mode;
        emit deviceModeChanged();
    }
    qDebug() << __FUNCTION__ << mDeviceMode;
    bool open_ret = false;
    switch (mDeviceMode) {
    case TestUsb:
        open_ret = doOpenUsb(mFd);
        break;
    case TestUvc:
        open_ret = doOpenUvc(mFd);
        break;
    }
    if (open_ret) {
        mIsOpen = true;
        emit stateChanged();
    }
}

void USBManager::testClose()
{
    switch (mDeviceMode) {
    case TestUsb:
        doCloseUsb();
        break;
    case TestUvc:
        doCloseUvc();
        break;
    }
    mIsOpen = false;
    emit stateChanged();
}

bool USBManager::doOpenUsb(int fd)
{
    libusb_device_handle *devh = nullptr;
    int usb_ret = 0;
    usb_ret = libusb_set_option(nullptr, LIBUSB_OPTION_NO_DEVICE_DISCOVERY, nullptr);
    if (usb_ret != LIBUSB_SUCCESS) {
        qDebug() << "libusb_set_option failed:" << usb_ret << libusb_strerror(usb_ret);
        return false;
    }
    usb_ret = libusb_init(&usbCtx);
    if (usb_ret < 0) {
        qDebug() << "libusb_init failed:" << usb_ret << libusb_strerror(usb_ret);
        return false;
    }
    usb_ret = libusb_wrap_sys_device(usbCtx, (intptr_t)fd, &devh);
    if (usb_ret < 0) {
        qDebug() << "libusb_wrap_sys_device failed:" << usb_ret << libusb_strerror(usb_ret);
        return false;
    } else if (!devh) {
        qDebug() << "libusb_wrap_sys_device returned invalid handle";
        return false;
    }
    usbHandle = devh;
    usbDevice = libusb_get_device(devh);
    deviceDescriptor(usbDevice);
    usb_ret = libusb_claim_interface(usbHandle, 0);
    if (usb_ret < 0) {
        qDebug() << "libusb_claim_interface error." << usb_ret << libusb_strerror(usb_ret);
        usbHandle = nullptr;
        usbDevice = nullptr;
        libusb_exit(usbCtx);
        usbCtx = nullptr;
        return false;
    }
    return true;
}

void USBManager::doCloseUsb()
{
    if (usbHandle) {
        libusb_release_interface(usbHandle, 0);
        usbHandle = nullptr;
    }
    usbDevice = nullptr;
    if (usbCtx) {
        libusb_exit(usbCtx);
        usbCtx = nullptr;
    }
}

bool USBManager::doOpenUvc(int fd)
{
    uvc_error_t uvc_ret = UVC_SUCCESS;
    uvc_ret = uvc_init2(&uvcCtx, nullptr);
    if (uvc_ret < 0) {
        qDebug() << "uvc_init failed:" << uvc_ret << uvc_strerror(uvc_ret);
        return false;
    }
    uvc_ret = uvc_wrap(fd, uvcCtx, &uvcHandle);
    if (uvc_ret < 0) {
        qDebug() << "uvc_wrap failed:" << uvc_ret << uvc_strerror(uvc_ret);
        return false;
    }
    int cur_width = 0;
    int cur_height = 0;
    uvc_frame_format cur_format = UVC_FRAME_FORMAT_UNKNOWN;
    qDebug() << "stream_ifs" << !!uvcHandle->info->stream_ifs;
    // 遍历格式
    if (uvcHandle->info->stream_ifs) {
        uvc_streaming_interface_t *stream_if = uvcHandle->info->stream_ifs;
        for (int i = 0; stream_if; stream_if = stream_if->next, i++)
        {
            qDebug() << "UVC Format i" << i;
            uvc_format_desc_t *fmt_desc = stream_if->format_descs;
            for (int j = 0; fmt_desc; fmt_desc = fmt_desc->next, j++)
            {
                qDebug() << "UVC Format j" << j << fmt_desc->bDefaultFrameIndex;
                switch (fmt_desc->bDescriptorSubtype)
                {
                case UVC_VS_FORMAT_UNCOMPRESSED:
                    qDebug() << "UVC_VS_FORMAT_UNCOMPRESSED";
                    break;
                case UVC_VS_FORMAT_MJPEG:
                    qDebug() << "UVC_VS_FORMAT_MJPEG";
                    break;
                default: continue;
                }
                uvc_frame_desc_t *frame_desc = fmt_desc->frame_descs;
                for (int k = 0; frame_desc; frame_desc = frame_desc->next, k++)
                {
                    qDebug() << QString("size: %1 x %2").arg(frame_desc->wWidth).arg(frame_desc->wHeight);
                    // 取第一个宽高来设置
                    if (cur_width == 0) {
                        cur_width = frame_desc->wWidth;
                        cur_height = frame_desc->wHeight;
                        cur_format = (fmt_desc->bDescriptorSubtype == UVC_VS_FORMAT_MJPEG) ? UVC_FRAME_FORMAT_MJPEG : UVC_FRAME_FORMAT_YUYV;
                    }
                }
            }
        }
    }
    // 按键回调，启动视频流后才会触发
    uvc_set_button_callback(uvcHandle, [](int button, int state, void *){
            // 1-1 按下, 1-0 弹起
            qDebug() << "uvc button callback" << button << state;
        }, nullptr);
    uvc_ret = uvc_get_stream_ctrl_format_size(uvcHandle, &uvcCtrl, cur_format, cur_width, cur_height, 0);
    if (uvc_ret < 0) {
        qDebug() << "uvc_get_stream_ctrl_format_size failed:" << uvc_ret << uvc_strerror(uvc_ret);
    }
    uvc_ret = uvc_start_streaming(uvcHandle, &uvcCtrl, [](struct uvc_frame *frame, void *userData){
            if (!frame) return;
            static int i = 0;
            qDebug() << "uvc stream callback" << frame->width << frame->height << frame->frame_format << QTime::currentTime() << i++;
            // return;
            USBManager *manager = static_cast<USBManager *>(userData);
            if (manager) {
                // TODO 数据拷贝出来放到解析的线程种处理
                uvc_frame_t *copy = uvc_allocate_frame(frame->data_bytes);
                if (!copy) return;
                uvc_error_t err = uvc_duplicate_frame(frame, copy);
                if (err) {
                    uvc_free_frame(copy);
                } else {
                    uvc_frame_t *rgb = uvc_allocate_frame(frame->data_bytes);
                    QImage image;
                    // TODO 暂时用rgb，后面把yuv处理加上
                    if (!rgb) {
                    } if (frame->frame_format == UVC_FRAME_FORMAT_MJPEG) {
                        uvc_error_t err = uvc_mjpeg2rgb(copy, rgb);
                        if (!err) {
                            image = QImage((const uchar *)rgb->data, rgb->width, rgb->height,
                                           rgb->width * 3, QImage::Format_RGB888);
                        }
                    } else if (frame->frame_format == UVC_FRAME_FORMAT_YUYV) {
                        uvc_error_t err = uvc_yuyv2rgb(copy, rgb);
                        if (!err) {
                            image = QImage((const uchar *)rgb->data, rgb->width, rgb->height,
                                           rgb->width * 3, QImage::Format_RGB888);
                        }
                    }
                    uvc_free_frame(copy);
                    uvc_free_frame(rgb);
                    manager->newFrame(image);
                }
            }
        }, this, 0);
    if (uvc_ret < 0) {
        qDebug() << "uvc_start_streaming failed:" << uvc_ret << uvc_strerror(uvc_ret);
    }
    return true;
}

void USBManager::doCloseUvc()
{
    qDebug() << __FUNCTION__;
    if (uvcHandle) {
        uvc_close(uvcHandle);
        uvcHandle = nullptr;
    }
    // uvcDevice = nullptr;
    if (uvcCtx) {
        uvc_exit(uvcCtx);
        uvcCtx = nullptr;
    }
}
