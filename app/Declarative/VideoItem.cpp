#include "VideoItem.h"
#include <QCoreApplication>
#include <QQuickWindow>
#include <QSGSimpleTextureNode>
#include <QTime>
#include <QDebug>

VideoItem::VideoItem(QQuickItem *parent)
    : QQuickItem{parent}
{
    setFlag(ItemHasContents, true);
}

void VideoItem::updateFrame(const QImage &image)
{
    static int i = 0;
    qDebug() << __FUNCTION__ << QTime::currentTime() << i++;
    viewImage = image;
    setImplicitWidth(viewImage.width());
    setImplicitHeight(viewImage.height());
    update();
}

QSGNode *VideoItem::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *)
{
    QSGSimpleTextureNode *node = dynamic_cast<QSGSimpleTextureNode *>(oldNode);

    if(!node){
        node = new QSGSimpleTextureNode();
    }

    // 计算纹理区域
    QRect node_rect = boundingRect().toRect();
    const double image_ratio = viewImage.width() / (double)viewImage.height();
    const double rect_ratio = node_rect.width() / (double)node_rect.height();
    // image_ratio>rect_ratio表示太窄了rect要根绝宽度重新适配高度，否则太矮了重新适配宽度
    if (image_ratio > rect_ratio) {
        const int new_height = node_rect.width() / image_ratio;
        node_rect.setY(node_rect.y() + (node_rect.height() - new_height) / 2);
        node_rect.setHeight(new_height);
    } else {
        const int new_width = image_ratio * node_rect.height();
        node_rect.setX(node_rect.x() + (node_rect.width() - new_width) / 2);
        node_rect.setWidth(new_width);
    }

    QSGTexture *m_texture = window()->createTextureFromImage(viewImage);
    node->setTexture(m_texture);
    node->setOwnsTexture(true);
    node->setRect(node_rect);
    node->markDirty(QSGNode::DirtyGeometry);
    node->markDirty(QSGNode::DirtyMaterial);

    return node;
}
