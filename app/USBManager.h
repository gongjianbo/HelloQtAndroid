#pragma once
#include <QObject>
#include "libusb.h"
#include "libuvc/libuvc.h"
#include "libuvc/libuvc_internal.h"

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

    // 设备标识
    QString getDeviceInfo() const;
    // 测试类型
    int getDeviceMode() const;
    // 设备打开状态
    bool getIsOpen() const;

public slots:
    // 设备连接
    void connectDevice(int fd, int vid, int pid, const QString &name);
    // 设备断开
    void disconnectDevice();
    // 打开
    void testOpen(int mode = TestNone);
    // 关闭
    void testClose();

private:
    // USB 测试
    bool doOpenUsb(int fd);
    void doCloseUsb();
    // UVC 测试
    bool doOpenUvc(int fd, int vid, int pid, int busNum, int devAddr, const QByteArray &usbFs);
    void doCloseUvc();

signals:
    void deviceInfoChanged();
    void deviceModeChanged();
    void stateChanged();

private:
    // 设备状态
    QString mDeviceInfo;
    int mDeviceMode{TestUvc};
    bool mIsOpen{false};
    // 设备信息
    int mFd{0};
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
    uvc_device_t *uvcDevice{nullptr};
    uvc_device_handle_t *uvcHandle{nullptr};
    uvc_stream_ctrl_t uvcCtrl;
    // 视频格式
    uvc_frame_format uvcFormat{UVC_FRAME_FORMAT_UNKNOWN};
    int uvcWidth{0};
    int uvcHeight{0};
};
