#pragma once
#include <QObject>
#include <QCamera>
#include <QVideoFrame>
#include <QVideoProbe>
#include <QAbstractVideoSurface>
#include <QAbstractVideoFilter>
#include <QVideoFilterRunnable>
#include <QThread>
#include <QTimer>
#include <QImage>
#include <QUrl>

// QAbstractVideoFilter测试
class CameraFilter : public QAbstractVideoFilter
{
    Q_OBJECT
    Q_PROPERTY(QObject *source READ getSource  WRITE setSource NOTIFY sourceChanged FINAL)
    Q_PROPERTY(int fps READ getFps WRITE setFps NOTIFY fpsChanged FINAL)
public:
    explicit CameraFilter(QObject *parent = nullptr);
    ~CameraFilter();

    QObject *getSource() const;
    void setSource(QObject *source);

    int getFps() const;
    void setFps(int fps);

    Q_INVOKABLE void capture();

    QVideoFilterRunnable *createFilterRunnable() override;

signals:
    void sourceChanged();
    void fpsChanged();
    void captureFinished(const QUrl &url);

private:
    friend class CameraFilterRunnable;
    QCamera *mCamera{nullptr};
    // 定时器统计fps
    QTimer *mTimer{nullptr};
    int mFps{0};
    int mFpsCounter{0};
    // 拍图标记
    bool mCapture{false};
};

class CameraFilterRunnable : public QVideoFilterRunnable
{
public:
    explicit CameraFilterRunnable(CameraFilter *filter);
    ~CameraFilterRunnable();

    QVideoFrame run(QVideoFrame *input, const QVideoSurfaceFormat &surfaceFormat, RunFlags flags) override;

private:
    CameraFilter *mFilter;
};
