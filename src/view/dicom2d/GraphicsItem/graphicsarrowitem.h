#ifndef GRAPHICSARROWITEM_H
#define GRAPHICSARROWITEM_H

#include "abstractpathitem.h"

/**
 * @brief The GraphicsArrowItem class
 * 箭头
 */
class GraphicsArrowItem : public AbstractPathItem
{
public:
    explicit GraphicsArrowItem(QGraphicsItem * parent = nullptr);
    void setActivePoint(const QPointF & point);
    void nextStage()
    {
        if (start != end) {
            currentStage = Final;
        }
    }

private:
    QPainterPath itemPath();
    bool isModified();

    QPointF start;
    QPointF end;
};

#endif // GRAPHICSARROWITEM_H
