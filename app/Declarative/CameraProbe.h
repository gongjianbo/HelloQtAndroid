#pragma once
#include <QObject>
#include <QCamera>
#include <QVideoFrame>
#include <QVideoProbe>
#include <QAbstractVideoSurface>
#include <QThread>
#include <QTimer>
#include <QImage>
#include <QUrl>

// QVideoProbe测试
class CameraProbe : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QObject *source READ getSource  WRITE setSource NOTIFY sourceChanged FINAL)
    Q_PROPERTY(int fps READ getFps WRITE setFps NOTIFY fpsChanged FINAL)
public:
    explicit CameraProbe(QObject *parent = nullptr);
    ~CameraProbe();

    QObject *getSource() const;
    void setSource(QObject *source);

    int getFps() const;
    void setFps(int fps);

    Q_INVOKABLE void capture();

signals:
    void sourceChanged();
    void fpsChanged();
    void captureFinished(const QUrl &url);

private:
    void doCapture(QVideoFrame frame);

private:
    QCamera *mCamera{nullptr};
    QVideoProbe *mProbe{nullptr};
    // 定时器统计fps
    QTimer *mTimer{nullptr};
    int mFps{0};
    int mFpsCounter{0};
    // 拍图标记
    bool mCapture{false};
};
