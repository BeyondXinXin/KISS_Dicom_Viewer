#include "graphicstextmarkitem.h"
#include "graphicssimpletextitem.h"

//----------------------------------------------------------------
GraphicsTextMarkItem::GraphicsTextMarkItem(QGraphicsItem * parent)
  : AbstractPathItem(parent)
{
    setFlags(flags() | QGraphicsItem::ItemDoesntPropagateOpacityToChildren | QGraphicsItem::ItemIgnoresTransformations);
    textItem->setFlag(QGraphicsItem::ItemIgnoresTransformations, false);
    textItem->setFlag(QGraphicsItem::ItemIgnoresParentOpacity);
    textItem->setFlag(QGraphicsItem::ItemIsMovable);
    textItem->setFlag(QGraphicsItem::ItemIsSelectable);
    setOpacity(0);
}

//----------------------------------------------------------------
void GraphicsTextMarkItem::setLabelItemText(const QString & text)
{
    textItem->setText(text);
}
