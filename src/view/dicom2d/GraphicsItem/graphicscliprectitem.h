#ifndef GRAPHICSCLIPRECTITEM_H
#define GRAPHICSCLIPRECTITEM_H

#include <QGraphicsRectItem>

class GraphicsClipRectItem : public QGraphicsRectItem
{
public:
    enum NodeName
    {
        TopLeft,
        TopMiddle,
        TopRight,
        MiddleRight,
        BottomRight,
        BottomMiddle,
        BottomLeft,
        MiddleLeft,
        NodeCount,
    };
    explicit GraphicsClipRectItem(QGraphicsItem * parent = nullptr);
    explicit GraphicsClipRectItem(QRectF & rect, QGraphicsItem * parent = nullptr);
    void setRect(const QRectF & rect);
    void setPen(const QPen & pen);
    QRectF getClipRect() const;
    void reverseCornerCursor();
    void reverseMiddleCursor();

protected:
    void paint(QPainter * painter,
               const QStyleOptionGraphicsItem * option, QWidget * widget = nullptr);

private:
    void init();
    void repositionNode();
    void setRectPrivate(const QRectF & rect);
    void setCornerCursor();
    void setMiddleCursor();
    QList<QGraphicsRectItem *> nodeList;
    bool corCursor;
    bool midCursor;
};

#endif // GRAPHICSCLIPRECTITEM_H
