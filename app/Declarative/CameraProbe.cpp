#include "CameraProbe.h"
#include "AppTool.h"
#include <QVideoSurfaceFormat>
#include <QDebug>

CameraProbe::CameraProbe(QObject *parent)
    : QObject{parent}
    , mTimer{new QTimer(this)}
{
    // 因为这是在主线程，容易被其他操作影响定时器精度
    connect(mTimer, &QTimer::timeout, this, [this]{
        setFps(mFpsCounter);
        mFpsCounter = 0;
        // qDebug() << "CameraProbe QTimer::timeout" << mFps;
    });
}

CameraProbe::~CameraProbe()
{

}

QObject *CameraProbe::getSource() const
{
    return mCamera;
}

void CameraProbe::setSource(QObject *source)
{
    if (source == mCamera)
        return;
    if (mCamera) {
        if (mProbe) {
            mProbe->deleteLater();
            mProbe = nullptr;
        }
    }
    if (!source)
        return;
    mCamera = qvariant_cast<QCamera*>(source->property("mediaObject"));
    if (!mCamera)
        return;

    mProbe = new QVideoProbe(this);
    mProbe->setSource(mCamera);
    connect(mProbe, &QVideoProbe::videoFrameProbed, this, [this](const QVideoFrame &frame){
        mFpsCounter++;
        if (mCapture) {
            // 如果是获取视频流，就不在主线程处理
            doCapture(frame);
            mCapture = false;
        }
    }, Qt::DirectConnection);
    connect(mCamera, &QCamera::statusChanged, this, [this](QCamera::Status status){
        qDebug() << "CameraProbe QCamera::statusChanged" << status;
        mFpsCounter = 0;
        mCapture = false;
        if (status == QCamera::ActiveStatus) {
            mTimer->start(1000);
        } else {
            mTimer->stop();
        }
        setFps(0);
    });
}

int CameraProbe::getFps() const
{
    return mFps;
}

void CameraProbe::setFps(int fps)
{
    if (fps == mFps)
        return;
    mFps = fps;
    emit fpsChanged();
}

void CameraProbe::capture()
{
    mCapture = true;
}

void CameraProbe::doCapture(QVideoFrame frame)
{
    if (!frame.isValid())
        return;
    QImage image;
    frame.map(QAbstractVideoBuffer::ReadOnly);
    if (frame.pixelFormat() == QVideoFrame::Format_ABGR32) {
        const int width = frame.width();
        const int height = frame.height();
        const int bytes = width * height * 4;
        image = QImage(width, height, QImage::Format_ARGB32);
        const uchar *in_bits = frame.bits();
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
        image = frame.image();
    }
    frame.unmap();
    // 图片还需要旋转才能和VideoOutput一致
    const QString path = AppTool::getInstance()->appDataPath() + "/capture_temp.jpg";
    image.save(path);
    qDebug() << __FUNCTION__ << frame << path << QThread::currentThread();
    emit captureFinished(QUrl::fromLocalFile(path));
}
