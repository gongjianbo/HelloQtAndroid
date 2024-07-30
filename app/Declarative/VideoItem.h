#pragma once
#include <QQuickItem>
#include <QImage>

// 视频播放
class VideoItem : public QQuickItem
{
    Q_OBJECT
public:
    explicit VideoItem(QQuickItem *parent = nullptr);

    Q_INVOKABLE void updateFrame(const QImage &image);

protected:
    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *) override;

private:
    // 当前缓存的渲染的图片
    QImage viewImage;
};
