#pragma once
#include <QObject>
#include "libusb.h"

class USBManager : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(USBManager)
    USBManager();
    Q_PROPERTY(bool isOpen READ getIsOpen NOTIFY deviceChanged)
    Q_PROPERTY(QString deviceInfo READ getDeviceInfo NOTIFY deviceChanged)
public:
    static USBManager *getInstance();
    ~USBManager();

    // 设备打开状态
    bool getIsOpen() const;
    // 设备标识
    QString getDeviceInfo() const;

public slots:
    void openDevice(int fd, int vid, int pid);
    void closeDevice();
    void testUsb();
    void testUvc();

private:
    bool doOpen(int fd);
    void doClose();

signals:
    void deviceChanged();

private:
    bool mIsOpen{false};
    QString mDeviceInfo;
    // 设备描述符
    int mFd{0};
    int mVid{0};
    int mPid{0};
    libusb_context *fdCtx{nullptr};
    // 设备列表
    libusb_device **mDeviceList{nullptr};
    // 当前设备
    libusb_device *mDevice{nullptr};
    libusb_device_handle *mHandle{nullptr};
};
