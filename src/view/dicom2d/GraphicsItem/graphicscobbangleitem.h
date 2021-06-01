#ifndef GRAPHICSCOBBANGLEITEM_H
#define GRAPHICSCOBBANGLEITEM_H

#include "abstractpathitem.h"
class GraphicsCrossItem;

class GraphicsCobbAngleItem : public AbstractPathItem
{
public:
    explicit GraphicsCobbAngleItem(QGraphicsItem * parent = nullptr);
    void setActivePoint(const QPointF & point);
    void nextStage();

private:
    void init();
    void updateTextItem();
    QPointF textItemPos();
    QPainterPath itemPath();
    bool isModified();
    void angle();

    GraphicsCrossItem * line1Vertex1;
    GraphicsCrossItem * line1Vertex2;
    GraphicsCrossItem * auxVertex;
    GraphicsCrossItem * line2Vertex1;
    GraphicsCrossItem * line2Vertex2;

    QPointF prevl1v1;
    QPointF prevl2v2;
    QPointF prevl1v2;
    QPointF prevl2v1;

    double angleCacheData;
};

#endif // GRAPHICSCOBBANGLEITEM_H
