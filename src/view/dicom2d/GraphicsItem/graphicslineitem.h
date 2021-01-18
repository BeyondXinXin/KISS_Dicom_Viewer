#ifndef GRAPHICSLINEITEM_H
#define GRAPHICSLINEITEM_H

#include "abstractpathitem.h"
#include <QGraphicsLineItem>
#include <QPen>
class GraphicsSimpleTextItem;
class GraphicsCrossItem;

class GraphicsLineItem : public AbstractPathItem {
  public:
    explicit GraphicsLineItem(QGraphicsItem *parent = nullptr);
    void setActivePoint(const QPointF &point);
    void nextStage() {
        currentStage = Final;
    }

  private:
    void init();
    void updateTextItem();
    QPointF textItemPos();
    bool isModified();
    QPainterPath itemPath();

  private:
    GraphicsCrossItem *crossItem1;
    GraphicsCrossItem *crossItem2;

    QPointF prev1;
    QPointF prev2;
};

#endif // GRAPHICSLINEITEM_H
