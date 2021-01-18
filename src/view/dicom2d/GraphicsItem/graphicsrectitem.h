#ifndef GRAPHICSRECTITEM_H
#define GRAPHICSRECTITEM_H

#include "abstractpathitem.h"
class GraphicsCrossItem;

class GraphicsRectItem : public AbstractPathItem {
  public:
    explicit GraphicsRectItem(QGraphicsItem *parent = nullptr);
    void setActivePoint(const QPointF &point);
    void nextStage() {
        currentStage = Final;
    }
    void recalPixInfo(const DicomImage *dcmImage);
    bool pixInfoUpdated() const {
        return updated;
    }

  private:
    void init();
    void updateTextItem();
    QPointF textItemPos();
    QPainterPath itemPath();
    bool isModified();

    GraphicsCrossItem *tl;
    GraphicsCrossItem *br;

    QPointF prevTl;
    QPointF prevBr;
    QPointF prevThis;

    double area;
    bool updated;
};

#endif // GRAPHICSRECTITEM_H
