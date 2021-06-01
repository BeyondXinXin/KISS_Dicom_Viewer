#ifndef GRAPHICSANGLEITEM_H
#define GRAPHICSANGLEITEM_H

#include "abstractpathitem.h"
class QGraphicsItem;
class GraphicsCrossItem;

/**
 * @brief The GraphicsAngleItem class
 * 角度
 */
class GraphicsAngleItem : public AbstractPathItem
{
public:
    explicit GraphicsAngleItem(QGraphicsItem * parent = nullptr);
    void setActivePoint(const QPointF & point);
    void nextStage();

private:
    void init();
    void updateTextItem();
    QPointF textItemPos();
    QPainterPath itemPath();
    bool isModified();

    GraphicsCrossItem * pointAngle;
    GraphicsCrossItem * point1;
    GraphicsCrossItem * point2;

    QPointF prevPAngle;
    QPointF prevP1;
    QPointF prevP2;
};

#endif // GRAPHICSANGLEITEM_H
