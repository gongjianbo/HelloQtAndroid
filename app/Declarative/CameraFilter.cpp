#include "CameraFilter.h"
#include "AppTool.h"
#include <QVideoSurfaceFormat>
#include <QDebug>

CameraFilter::CameraFilter(QObject *parent)
    : QAbstractVideoFilter{parent}
    , mTimer{new QTimer(this)}
{
    // 因为这是在主线程，容易被其他操作影响定时器精度
    connect(mTimer, &QTimer::timeout, this, [this]{
        setFps(mFpsCounter);
        mFpsCounter = 0;
        // qDebug() << "CameraFilter QTimer::timeout" << mFps;
    });
}

CameraFilter::~CameraFilter()
{

}

QObject *CameraFilter::getSource() const
{
    return mCamera;
}

void CameraFilter::setSource(QObject *source)
{
    if (source == mCamera)
        return;
    if (mCamera) {
    }
    if (!source)
        return;
    mCamera = qvariant_cast<QCamera*>(source->property("mediaObject"));
    if (!mCamera)
        return;

    connect(mCamera, &QCamera::stateChanged, this, [this](QCamera::State state){
        qDebug() << "CameraFilter QCamera::stateChanged" << state;
        mFpsCounter = 0;
        mCapture = false;
        if (state == QCamera::ActiveState) {
            mTimer->start(1000);
        } else {
            mTimer->stop();
        }
        setFps(0);
    });
}

int CameraFilter::getFps() const
{
    return mFps;
}

void CameraFilter::setFps(int fps)
{
    if (fps == mFps)
        return;
    mFps = fps;
    emit fpsChanged();
}

QVideoFilterRunnable *CameraFilter::createFilterRunnable()
{
    return new CameraFilterRunnable(this);
}

void CameraFilter::capture()
{
    mCapture = true;
}

CameraFilterRunnable::CameraFilterRunnable(CameraFilter *filter)
    : mFilter(filter)
{

}

CameraFilterRunnable::~CameraFilterRunnable()
{

}

QVideoFrame CameraFilterRunnable::run(QVideoFrame *input, const QVideoSurfaceFormat &surfaceFormat, RunFlags flags)
{
    Q_UNUSED(surfaceFormat)
    Q_UNUSED(flags)
    if (!input || !input->isValid())
        return QVideoFrame();
    mFilter->mFpsCounter++;
    // 目前只是用来拍图，所以不用每帧都处理
    if (!mFilter->mCapture) {
        return *input;
    }
    QImage image;
    input->map(QAbstractVideoBuffer::ReadOnly);
    if (input->pixelFormat() == QVideoFrame::Format_ABGR32) {
        const int width = input->width();
        const int height = input->height();
        const int bytes = width * height * 4;
        image = QImage(width, height, QImage::Format_ARGB32);
        const uchar *in_bits = input->bits();
        uchar *out_bits = image.bits();

        static const size_t i1 = (Q_BYTE_ORDER == Q_LITTLE_ENDIAN ? 2 : 0);
        static const size_t i2 = (Q_BYTE_ORDER == Q_LITTLE_ENDIAN ? 1 : 3);
        static const size_t i3 = (Q_BYTE_ORDER == Q_LITTLE_ENDIAN ? 0 : 2);
        static const size_t i4 = (Q_BYTE_ORDER == Q_LITTLE_ENDIAN ? 3 : 1);

        for (int i = 0; i < bytes;)
        {
            out_bits[0] = in_bits[i1];
            out_bits[1] = in_bits[i2];
            out_bits[2] = in_bits[i3];
            out_bits[3] = in_bits[i4];
            in_bits += 4;
            out_bits += 4;
            i += 4;
        }
    } else {
        image = input->image();
    }
    input->unmap();
    // 同步渲染时的翻转设置
    bool v_mirror = false;
    bool h_mirror = false;
    if (surfaceFormat.scanLineDirection() == QVideoSurfaceFormat::BottomToTop) {
        v_mirror = true;
    }
    if (surfaceFormat.property("mirrored").toBool()) {
        h_mirror = true;
    }
    image = image.mirrored(h_mirror, v_mirror);
    if (mFilter->mCapture) {
        // 图片还需要旋转才能和VideoOutput一致
        const QString path = AppTool::getInstance()->appDataPath() + "/capture_temp.jpg";
        image.save(path);
        qDebug() << __FUNCTION__ << *input << surfaceFormat << path << QThread::currentThread();
        emit mFilter->captureFinished(QUrl::fromLocalFile(path));
        mFilter->mCapture = false;
    }
    return *input;
}
