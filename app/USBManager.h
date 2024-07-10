#pragma once
#include <QObject>
#include "libusb.h"

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
    void connectDevice(int fd, int vid, int pid);
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

signals:
    void deviceInfoChanged();
    void deviceModeChanged();
    void stateChanged();

private:
    // 设备状态
    QString mDeviceInfo;
    int mDeviceMode{TestUsb};
    bool mIsOpen{false};
    // 设备描述符
    int mFd{0};
    int mVid{0};
    int mPid{0};

    // USB 测试
    libusb_context *usbCtx{nullptr};
    // 当前设备
    libusb_device *usbDevice{nullptr};
    libusb_device_handle *usbHandle{nullptr};
};
