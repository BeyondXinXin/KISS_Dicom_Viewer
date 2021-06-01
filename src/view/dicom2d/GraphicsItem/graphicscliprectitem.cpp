#include "graphicscliprectitem.h"
#include "global/stable.h"

#define NODE_SIZE 32
#define MIN_CLIP_SIZE 300

//----------------------------------------------------------------
GraphicsClipRectItem::GraphicsClipRectItem(QGraphicsItem * parent)
  : corCursor(true)
  , midCursor(true)
  , QGraphicsRectItem(parent)
{
    init();
}

//----------------------------------------------------------------
GraphicsClipRectItem::GraphicsClipRectItem(QRectF & rect, QGraphicsItem * parent)
  : corCursor(true)
  , midCursor(true)
  , QGraphicsRectItem(rect, parent)
{
    init();
}

//----------------------------------------------------------------
void GraphicsClipRectItem::init()
{
    setFlag(QGraphicsItem::ItemIsMovable);
    for (int i = 0; i < NodeCount; ++i) {
        QGraphicsRectItem * node = new QGraphicsRectItem(QRectF(0, 0, NODE_SIZE, NODE_SIZE), this);
        node->setAcceptHoverEvents(true);
        node->setFlag(QGraphicsItem::ItemIsMovable);
        nodeList.append(node);
    }
    setCornerCursor();
    setMiddleCursor();
    repositionNode();
}

//----------------------------------------------------------------
void GraphicsClipRectItem::reverseCornerCursor()
{
    corCursor = !corCursor;
    setCornerCursor();
}

//----------------------------------------------------------------
void GraphicsClipRectItem::reverseMiddleCursor()
{
    midCursor = !midCursor;
    setMiddleCursor();
}

//----------------------------------------------------------------
void GraphicsClipRectItem::setCornerCursor()
{
    nodeList[TopLeft]->setCursor(corCursor ? QCursor(Qt::SizeFDiagCursor) : QCursor(Qt::SizeBDiagCursor));
    nodeList[TopRight]->setCursor(corCursor ? QCursor(Qt::SizeBDiagCursor) : QCursor(Qt::SizeFDiagCursor));
    nodeList[BottomRight]->setCursor(corCursor ? QCursor(Qt::SizeFDiagCursor) : QCursor(Qt::SizeBDiagCursor));
    nodeList[BottomLeft]->setCursor(corCursor ? QCursor(Qt::SizeBDiagCursor) : QCursor(Qt::SizeFDiagCursor));
}

//----------------------------------------------------------------
void GraphicsClipRectItem::setMiddleCursor()
{
    nodeList[TopMiddle]->setCursor(midCursor ? QCursor(Qt::SizeVerCursor) : QCursor(Qt::SizeHorCursor));
    nodeList[MiddleRight]->setCursor(midCursor ? QCursor(Qt::SizeHorCursor) : QCursor(Qt::SizeVerCursor));
    nodeList[BottomMiddle]->setCursor(midCursor ? QCursor(Qt::SizeVerCursor) : QCursor(Qt::SizeHorCursor));
    nodeList[MiddleLeft]->setCursor(midCursor ? QCursor(Qt::SizeHorCursor) : QCursor(Qt::SizeVerCursor));
}

//----------------------------------------------------------------
void GraphicsClipRectItem::repositionNode()
{
    QRectF rect = this->rect();
    nodeList[TopLeft]->setPos(rect.left() - (NODE_SIZE / 2),
                              rect.top() - (NODE_SIZE / 2));
    nodeList[TopMiddle]->setPos(rect.left() + (rect.width() - NODE_SIZE) / 2,
                                rect.top() - (NODE_SIZE / 2));
    nodeList[TopRight]->setPos(rect.right() - NODE_SIZE / 2,
                               rect.top() - (NODE_SIZE / 2));
    nodeList[MiddleRight]->setPos(rect.right() - NODE_SIZE / 2,
                                  rect.top() + (rect.height() - NODE_SIZE) / 2);
    nodeList[BottomRight]->setPos(rect.right() - NODE_SIZE / 2,
                                  rect.bottom() - NODE_SIZE / 2);
    nodeList[BottomMiddle]->setPos(rect.left() + (rect.width() - NODE_SIZE) / 2,
                                   rect.bottom() - NODE_SIZE / 2);
    nodeList[BottomLeft]->setPos(rect.left() - (NODE_SIZE / 2),
                                 rect.bottom() - NODE_SIZE / 2);
    nodeList[MiddleLeft]->setPos(rect.left() - (NODE_SIZE / 2),
                                 rect.top() + (rect.height() - NODE_SIZE) / 2);
}

//----------------------------------------------------------------
void GraphicsClipRectItem::setRect(const QRectF & rect)
{
    QPointF pos = mapFromParent(rect.topLeft());
    QGraphicsRectItem::setRect(pos.x(), pos.y(), rect.width(), rect.height());
    repositionNode();
}

//----------------------------------------------------------------
void GraphicsClipRectItem::setRectPrivate(const QRectF & rect)
{
    QGraphicsRectItem::setRect(rect);
    repositionNode();
}

//----------------------------------------------------------------
void GraphicsClipRectItem::setPen(const QPen & pen)
{
    for (int i = 0; i < NodeCount; ++i) {
        nodeList.at(i)->setBrush(QBrush(pen.color()));
    }
    QGraphicsRectItem::setPen(pen);
}

//----------------------------------------------------------------
QRectF GraphicsClipRectItem::getClipRect() const
{
    return QRectF(mapToParent(nodeList[TopLeft]->pos() + QPointF(NODE_SIZE / 2, NODE_SIZE / 2)),
                  mapToParent(nodeList[BottomRight]->pos() + QPointF(NODE_SIZE / 2, NODE_SIZE / 2)));
}

//----------------------------------------------------------------
void GraphicsClipRectItem::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    QRectF rect = this->rect();
    if (nodeList[TopLeft]->pos() != QPointF(rect.left() - (NODE_SIZE / 2), rect.top() - (NODE_SIZE / 2))) {
        if (nodeList[TopLeft]->x() + MIN_CLIP_SIZE > nodeList[TopRight]->x()) {
            nodeList[TopLeft]->setX(nodeList[TopRight]->x() - MIN_CLIP_SIZE);
        }
        if (nodeList[TopLeft]->y() + MIN_CLIP_SIZE > nodeList[BottomLeft]->y()) {
            nodeList[TopLeft]->setY(nodeList[BottomLeft]->y() - MIN_CLIP_SIZE);
        }
        rect.setTopLeft(nodeList[TopLeft]->pos() + QPointF(NODE_SIZE / 2, NODE_SIZE / 2));
        setRectPrivate(rect);
    } else if (nodeList[TopMiddle]->pos() != QPointF(rect.left() + (rect.width() - NODE_SIZE) / 2, rect.top() - (NODE_SIZE / 2))) {
        if (nodeList[TopMiddle]->y() + MIN_CLIP_SIZE > nodeList[BottomMiddle]->y()) {
            nodeList[TopMiddle]->setY(nodeList[BottomMiddle]->y() - MIN_CLIP_SIZE);
        }
        rect.setTop(nodeList[TopMiddle]->pos().y() + NODE_SIZE / 2);
        setRectPrivate(rect);
    } else if (nodeList[TopRight]->pos() != QPointF(rect.right() - NODE_SIZE / 2, rect.top() - (NODE_SIZE / 2))) {
        if (nodeList[TopLeft]->x() + MIN_CLIP_SIZE > nodeList[TopRight]->x()) {
            nodeList[TopRight]->setX(nodeList[TopLeft]->x() + MIN_CLIP_SIZE);
        }
        if (nodeList[TopRight]->y() + MIN_CLIP_SIZE > nodeList[BottomRight]->y()) {
            nodeList[TopRight]->setY(nodeList[BottomRight]->y() - MIN_CLIP_SIZE);
        }
        rect.setTopRight(nodeList[TopRight]->pos() + QPointF(NODE_SIZE / 2, NODE_SIZE / 2));
        setRectPrivate(rect);
    } else if (nodeList[MiddleRight]->pos() != QPointF(rect.right() - NODE_SIZE / 2, rect.top() + (rect.height() - NODE_SIZE) / 2)) {
        if (nodeList[MiddleLeft]->x() + MIN_CLIP_SIZE > nodeList[MiddleRight]->x()) {
            nodeList[MiddleRight]->setX(nodeList[MiddleLeft]->x() + MIN_CLIP_SIZE);
        }
        rect.setRight(nodeList[MiddleRight]->pos().x() + NODE_SIZE / 2);
        setRectPrivate(rect);
    } else if (nodeList[BottomRight]->pos() != QPointF(rect.right() - NODE_SIZE / 2, rect.bottom() - NODE_SIZE / 2)) {
        if (nodeList[BottomLeft]->x() + MIN_CLIP_SIZE > nodeList[BottomRight]->x()) {
            nodeList[BottomRight]->setX(nodeList[BottomLeft]->x() + MIN_CLIP_SIZE);
        }
        if (nodeList[TopRight]->y() + MIN_CLIP_SIZE > nodeList[BottomRight]->y()) {
            nodeList[BottomRight]->setY(nodeList[TopRight]->y() + MIN_CLIP_SIZE);
        }
        rect.setBottomRight(nodeList[BottomRight]->pos() + QPointF(NODE_SIZE / 2, NODE_SIZE / 2));
        setRectPrivate(rect);
    } else if (nodeList[BottomMiddle]->pos() != QPointF(rect.left() + (rect.width() - NODE_SIZE) / 2, rect.bottom() - NODE_SIZE / 2)) {
        if (nodeList[TopMiddle]->y() + MIN_CLIP_SIZE > nodeList[BottomMiddle]->y()) {
            nodeList[BottomMiddle]->setY(nodeList[TopMiddle]->y() + MIN_CLIP_SIZE);
        }
        rect.setBottom(nodeList[BottomMiddle]->y() + NODE_SIZE / 2);
        setRectPrivate(rect);
    } else if (nodeList[BottomLeft]->pos() != QPointF(rect.left() - (NODE_SIZE / 2), rect.bottom() - NODE_SIZE / 2)) {
        if (nodeList[BottomLeft]->x() + MIN_CLIP_SIZE > nodeList[BottomRight]->x()) {
            nodeList[BottomLeft]->setX(nodeList[BottomRight]->x() - MIN_CLIP_SIZE);
        }
        if (nodeList[TopLeft]->y() + MIN_CLIP_SIZE > nodeList[BottomLeft]->y()) {
            nodeList[BottomLeft]->setY(nodeList[TopLeft]->y() + MIN_CLIP_SIZE);
        }
        rect.setBottomLeft(nodeList[BottomLeft]->pos() + QPointF(NODE_SIZE / 2, NODE_SIZE / 2));
        setRectPrivate(rect);
    } else if (nodeList[MiddleLeft]->pos() != QPointF(rect.left() - (NODE_SIZE / 2), rect.top() + (rect.height() - NODE_SIZE) / 2)) {
        if (nodeList[MiddleLeft]->x() + MIN_CLIP_SIZE > nodeList[MiddleRight]->x()) {
            nodeList[MiddleLeft]->setX(nodeList[MiddleRight]->x() - MIN_CLIP_SIZE);
        }
        rect.setLeft(nodeList[MiddleLeft]->pos().x() + NODE_SIZE / 2);
        setRectPrivate(rect);
    }
    QGraphicsRectItem::paint(painter, option, widget);
    this->scene()->update();
}
