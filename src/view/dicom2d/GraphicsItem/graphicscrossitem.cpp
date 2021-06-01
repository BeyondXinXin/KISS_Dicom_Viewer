#include "graphicscrossitem.h"

#include <engine/KissEngine>
#include <global/KissGlobal>

#define NORMAL_COLOR Qt::red
#define HIGHLIGHT_COLOR Qt::yellow

//----------------------------------------------------------------
GraphicsCrossItem::GraphicsCrossItem(QGraphicsItem * parent, double crossSize)
  : size(crossSize)
  , QGraphicsPathItem(parent)
{
    setPen(QPen(NORMAL_COLOR));
    setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIgnoresTransformations);
    setAcceptHoverEvents(true);
    QPainterPath path;
    path.moveTo(-size / 2, 0);
    path.lineTo(size / 2, 0);
    path.moveTo(0, -size / 2);
    path.lineTo(0, size / 2);
    setPath(path);
}

//----------------------------------------------------------------
void GraphicsCrossItem::setHighlight(bool yes)
{
    setPen(QPen(yes ? HIGHLIGHT_COLOR : NORMAL_COLOR));
}

//----------------------------------------------------------------
QSizeF GraphicsCrossItem::crossSize()
{
    return QSizeF(size, size);
}
