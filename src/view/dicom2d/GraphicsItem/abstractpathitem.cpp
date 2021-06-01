#include "abstractpathitem.h"
#include "graphicssimpletextitem.h"

#include <engine/KissEngine>
#include <global/KissGlobal>

#define CURSOR_SIZE 8.0
#define NORMAL_COLOR Qt::green
#define HIGHLIGHT_COLOR Qt::yellow
#define SELECTED_COLOR Qt::red

//----------------------------------------------------------------
AbstractPathItem::AbstractPathItem(QGraphicsItem * parent)
  : QGraphicsPathItem(parent)
{
    xSpacing = -1.0;
    ySpacing = -1.0;
    zoomFactor = 1.0;
    hoverEnter = false;
    init();
}

//----------------------------------------------------------------
void AbstractPathItem::init()
{
    stockPen.setColor(NORMAL_COLOR);
    stockPen.setWidthF(zoomFactor);
    setPen(stockPen);
    textItem = new GraphicsSimpleTextItem(this);
    setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemClipsToShape);
    setAcceptHoverEvents(true);
    currentStage = First;
}

//----------------------------------------------------------------
void AbstractPathItem::setFont(const QFont & font)
{
    textItem->setFont(font);
}

//----------------------------------------------------------------
void AbstractPathItem::paint(
  QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    if (isSelected()) {
        stockPen.setColor(SELECTED_COLOR);
        setPen(stockPen);
        textItem->setHighlight(true);
    } else if (hoverEnter) {
        stockPen.setColor(HIGHLIGHT_COLOR);
        setPen(stockPen);
        textItem->setHighlight(true);
    } else {
        stockPen.setColor(NORMAL_COLOR);
        setPen(stockPen);
        textItem->setHighlight(false);
    }
    if (isModified()) {
        setPath(itemPath());
        updateTextItem();
        textItem->setPos(textItemPos());
    }
    QGraphicsPathItem::paint(painter, option, widget);
}

//----------------------------------------------------------------
void AbstractPathItem::setZoomFactor(const double & factor)
{
    zoomFactor = 1 / factor;
    stockPen.setWidthF(zoomFactor);
}

//----------------------------------------------------------------
void AbstractPathItem::setPixelSpacing(const double & x, const double & y)
{
    xSpacing = x;
    ySpacing = y;
}

//----------------------------------------------------------------
void AbstractPathItem::hoverEnterEvent(QGraphicsSceneHoverEvent * event)
{
    hoverEnter = true;
    QGraphicsPathItem::hoverEnterEvent(event);
}

//----------------------------------------------------------------
void AbstractPathItem::hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
{
    hoverEnter = false;
    QGraphicsPathItem::hoverLeaveEvent(event);
}
