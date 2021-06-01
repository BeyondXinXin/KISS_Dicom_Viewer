#ifndef GRAPHICSTEXTMARKITEM_H
#define GRAPHICSTEXTMARKITEM_H

#include "abstractpathitem.h"

class GraphicsTextMarkItem : public AbstractPathItem
{
public:
    GraphicsTextMarkItem(QGraphicsItem * parent);

    void setLabelItemText(const QString & text);
    void setActivePoint(const QPointF & /*point*/)
    {
    }

private:
    QPointF textItemPos()
    {
        return QPointF(0, 0);
    }
};

#endif // GRAPHICSTEXTMARKITEM_H
