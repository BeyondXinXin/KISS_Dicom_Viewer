#ifndef GRAPHICSSIMPLETEXTITEM_H
#define GRAPHICSSIMPLETEXTITEM_H

#include <QBrush>
#include <QGraphicsSimpleTextItem>

class GraphicsSimpleTextItem : public QGraphicsSimpleTextItem
{
public:
    explicit GraphicsSimpleTextItem(QGraphicsItem * parent);
    void setHighlight(bool yes);
};

#endif // GRAPHICSSIMPLETEXTITEM_H
