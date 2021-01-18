#ifndef GRAPHICSSIMPLETEXTITEM_H
#define GRAPHICSSIMPLETEXTITEM_H

#include <QGraphicsSimpleTextItem>
#include <QBrush>

class GraphicsSimpleTextItem : public QGraphicsSimpleTextItem
{
public:
    explicit GraphicsSimpleTextItem(QGraphicsItem* parent);
    void setHighlight(bool yes);
};

#endif // GRAPHICSSIMPLETEXTITEM_H
