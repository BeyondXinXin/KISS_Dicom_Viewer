#include "graphicslineitem.h"
#include "abstractpathitem.h"
#include "graphicscrossitem.h"
#include "graphicssimpletextitem.h"
#include "math.h"

//----------------------------------------------------------------
GraphicsLineItem::GraphicsLineItem(QGraphicsItem * parent)
  : AbstractPathItem(parent)
{
    init();
}

//----------------------------------------------------------------
void GraphicsLineItem::init()
{
    crossItem1 = new GraphicsCrossItem(this);
    crossItem2 = new GraphicsCrossItem(this);
}

//----------------------------------------------------------------
void GraphicsLineItem::setActivePoint(const QPointF & point)
{
    crossItem2->setPos(point);
    update();
}

//----------------------------------------------------------------
bool GraphicsLineItem::isModified()
{
    if (crossItem1->pos() != prev1 || crossItem2->pos() != prev2) {
        prev1 = crossItem1->pos();
        prev2 = crossItem2->pos();
        return true;
    }
    return false;
}

//----------------------------------------------------------------
void GraphicsLineItem::updateTextItem()
{
    textItem->setText(QString());
    if (xSpacing < 0 || ySpacing < 0) {
        return;
    }
    QPointF p1 = mapToParent(crossItem1->pos());
    QPointF p2 = mapToParent(crossItem2->pos());
    double deltaX = (p1.x() - p2.x()) * xSpacing;
    double deltaY = (p1.y() - p2.y()) * ySpacing;
    textItem->setText(QObject::tr("%1 mm").arg(sqrt(deltaX * deltaX + deltaY * deltaY), 0, 'f', 2));
}

//----------------------------------------------------------------
QPointF GraphicsLineItem::textItemPos()
{
    if (crossItem1->x() > crossItem2->x()) {
        return QPointF(crossItem1->x() + crossItem1->crossSize().width(), crossItem1->y());
    } else {
        return QPointF(crossItem2->x() + crossItem2->crossSize().width(), crossItem2->y());
    }
}

//----------------------------------------------------------------
QPainterPath GraphicsLineItem::itemPath()
{
    QPainterPath line;
    line.moveTo(crossItem1->pos());
    line.lineTo(crossItem2->pos());
    return line;
}
