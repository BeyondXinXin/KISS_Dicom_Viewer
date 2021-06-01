#ifndef GRAPHICSELLIPSEITEM_H
#define GRAPHICSELLIPSEITEM_H

#include "abstractpathitem.h"
class GraphicsCrossItem;

class GraphicsEllipseItem : public AbstractPathItem
{
public:
    explicit GraphicsEllipseItem(QGraphicsItem * parent = nullptr);
    void setActivePoint(const QPointF & point);
    void nextStage()
    {
        currentStage = Final;
    }
    void recalPixInfo(const DicomImage * dcmImage);
    bool pixInfoUpdated() const
    {
        return updated;
    }

private:
    void init();
    void updateTextItem();
    QPointF textItemPos();
    bool isModified();
    QPainterPath itemPath();

    GraphicsCrossItem * tl;
    GraphicsCrossItem * br;

    QPointF prevTl;
    QPointF prevBr;
    QPointF prevThis;

    double area;
    bool updated;
};

#endif // GRAPHICSELLIPSEITEM_H
