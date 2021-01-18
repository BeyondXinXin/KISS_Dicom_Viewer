#ifndef GRAPHICSTUMORITEM_H
#define GRAPHICSTUMORITEM_H

#include "abstractpathitem.h"
#include <view/KissView>

class GraphicsTumorItem : public AbstractPathItem {
  public:
    GraphicsTumorItem(QGraphicsItem *parent = nullptr);

    void setActivePoint(const QPointF &) {}
    void setPoints(const QList<QPoint> &starts, const QList<QPoint> &ends) {
        m_starts = starts;
        m_ends = ends;
        modified = true;
    }
    void recalPixInfo(const short **vol, int width, int height,
                      int slice, int index, ViewType type);

  private:
    QPointF textItemPos();
    QPainterPath itemPath();
    bool isModified() {
        return modified;
    }

    QList<QPoint> m_starts;
    QList<QPoint> m_ends;
    QPainterPath path;
    bool modified;
};

#endif // GRAPHICSTUMORITEM_H
