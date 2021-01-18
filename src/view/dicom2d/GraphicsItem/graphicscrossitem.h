#ifndef GRAPHICSCROSSITEM_H
#define GRAPHICSCROSSITEM_H

#include <QGraphicsPathItem>

class GraphicsCrossItem : public QGraphicsPathItem {
  public:
    explicit GraphicsCrossItem(QGraphicsItem *parent = nullptr,
                               double crossSize = 10);
    void setHighlight(bool yes);
    QSizeF crossSize();

  private:
    double size;
};

#endif // GRAPHICSCROSSITEM_H
