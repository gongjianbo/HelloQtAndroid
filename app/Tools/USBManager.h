#pragma once
#include <QObject>
#include <QImage>
#include "libusb.h"
#include "libuvc/libuvc.h"
#include "libuvc/libuvc_internal.h"

// USB 操作
class USBManager : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(USBManager)
public:
    enum DeviceMode {
        TestNone = 0x00,
        TestUsb,
        TestUvc
    };
    Q_ENUM(DeviceMode)
private:
    USBManager();
    Q_PROPERTY(QString deviceInfo READ getDeviceInfo NOTIFY deviceInfoChanged)
    Q_PROPERTY(int deviceMode READ getDeviceMode NOTIFY deviceModeChanged)
    Q_PROPERTY(bool isOpen READ getIsOpen NOTIFY stateChanged)
public:
    static USBManager *getInstance();
    ~USBManager();
    // 动态注册 JNI 接口
    void initJNI();

    // 设备标识
    QString getDeviceInfo() const;
    // 测试类型
    int getDeviceMode() const;
    // 设备打开状态
    bool getIsOpen() const;

public slots:
    // 设备连接
    void onDeviceAttach(int fd, int vid, int pid, int deviceType, const QString &deviceName, const QString &productName);
    // 设备断开
    void onDeviceDetach(int fd);
    // 打开
    void testOpen(int mode = TestNone);
    // 关闭
    void testClose();

private:
    // USB 测试
    bool doOpenUsb(int fd);
    void doCloseUsb();
    // UVC 测试
    bool doOpenUvc(int fd);
    void doCloseUvc();

signals:
    void deviceInfoChanged();
    void deviceModeChanged();
    void stateChanged();
    void newFrame(const QImage &image);

private:
    // 设备状态
    QString mDeviceInfo;
    int mDeviceMode{TestUsb};
    bool mIsOpen{false};
    // 设备信息
    int mFd{0};
    int mDupFd{0};
    int mVid{0};
    int mPid{0};
    int mBusNum{0};
    int mDevAddr{0};
    QByteArray mUsbFs;

    // USB 测试
    libusb_context *usbCtx{nullptr};
    // 当前设备
    libusb_device *usbDevice{nullptr};
    libusb_device_handle *usbHandle{nullptr};

    // UVC 测试
    uvc_context_t *uvcCtx{nullptr};
    // 当前设备
    // uvc_device_t *uvcDevice{nullptr};
    uvc_device_handle_t *uvcHandle{nullptr};
    uvc_stream_ctrl_t uvcCtrl;
    // 视频格式
    uvc_frame_format uvcFormat{UVC_FRAME_FORMAT_UNKNOWN};
    int uvcWidth{0};
    int uvcHeight{0};
};
