#pragma once
#include <QObject>

class USBManager : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(USBManager)
    USBManager();
    Q_PROPERTY(bool isOpen READ getIsOpen NOTIFY deviceChanged)
    Q_PROPERTY(QString deviceName READ getDeviceName NOTIFY deviceChanged)
public:
    static USBManager *getInstance();
    ~USBManager();

    // 设备打开状态
    bool getIsOpen() const;
    // 设备标识
    QString getDeviceName() const;

private:
    void jniRegister();

public slots:
    void setDevice(const bool isOpen, const QString &deviceName);

signals:
    void deviceChanged();

private:
    bool mIsOpen{false};
    QString mDeviceName;
};
