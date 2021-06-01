#include "graphicscobbangleitem.h"
#include "graphicscrossitem.h"
#include "graphicssimpletextitem.h"




#include "math.h"

//----------------------------------------------------------------
GraphicsCobbAngleItem::GraphicsCobbAngleItem(QGraphicsItem * parent)
  : AbstractPathItem(parent)
{
    init();
}

//----------------------------------------------------------------
void GraphicsCobbAngleItem::init()
{
    line1Vertex1 = new GraphicsCrossItem(this);
    line1Vertex2 = new GraphicsCrossItem(this);
    auxVertex = new GraphicsCrossItem(this);
    line2Vertex1 = new GraphicsCrossItem(this);
    line2Vertex2 = new GraphicsCrossItem(this);
    currentStage = First;
}

//----------------------------------------------------------------
bool GraphicsCobbAngleItem::isModified()
{
    if (currentStage != Final || line1Vertex1->pos() != prevl1v1 || line1Vertex2->pos() != prevl1v2 || line2Vertex1->pos() != prevl2v1 || line2Vertex2->pos() != prevl2v2) {
        prevl1v1 = line1Vertex1->pos();
        prevl1v2 = line1Vertex2->pos();
        prevl2v1 = line2Vertex1->pos();
        prevl2v2 = line2Vertex2->pos();
        return true;
    }
    return false;
}

//----------------------------------------------------------------
void GraphicsCobbAngleItem::setActivePoint(const QPointF & point)
{
    switch (currentStage) {
    case First:
        line1Vertex2->setPos(point);
        break;
    case Second:
        line2Vertex1->setPos(point);
        line2Vertex2->setPos(point);
        break;
    case Third:
        line2Vertex2->setPos(point);
        break;
    }
}

//----------------------------------------------------------------
void GraphicsCobbAngleItem::nextStage()
{
    switch (currentStage) {
    case First:
        if (line1Vertex2->pos() != line1Vertex1->pos()) {
            currentStage = Second;
        }
        break;
    case Second:
        currentStage = Third;
        break;
    case Third:
        if (line2Vertex2->pos() != line2Vertex1->pos()) {
            currentStage = Final;
        }
        break;
    }
}

//----------------------------------------------------------------
void GraphicsCobbAngleItem::updateTextItem()
{
    if (line2Vertex1->pos() == line2Vertex2->pos()) {
        return;
    }
    textItem->setText(QObject::tr("%1 D").arg(angleCacheData > 90.0 ? (180.0 - angleCacheData) : angleCacheData, 0, 'f', 2));
}

//----------------------------------------------------------------
QPointF GraphicsCobbAngleItem::textItemPos()
{
    if (line2Vertex1->pos() == line2Vertex2->pos()) {
        return QPointF();
    }
    QPointF vector1;
    QPointF vector2;
    if (angleCacheData <= 90.0) {
        vector1 = auxVertex->pos() - line1Vertex1->pos();
        vector2 = line1Vertex2->pos() - line1Vertex1->pos();
    } else {
        vector1 = auxVertex->pos() - line1Vertex2->pos();
        vector2 = line1Vertex1->pos() - line1Vertex2->pos();
    }
    if ((vector1.x() < 0 || vector1.y() < 0) && (vector2.x() < 0 || vector2.y() < 0)) {
        if (angleCacheData <= 90.0)
            return QPointF(line1Vertex1->pos().x() + line1Vertex1->crossSize().width(),
                           line1Vertex1->pos().y());
        else
            return QPointF(line1Vertex2->pos().x() + line1Vertex2->crossSize().width(),
                           line1Vertex2->pos().y());
    } else if ((vector1.x() < 0 || vector1.y() > 0) && (vector2.x() < 0 || vector2.y() > 0)) {
        if (angleCacheData <= 90.0)
            return QPointF(line1Vertex1->pos().x() + line1Vertex1->crossSize().width(),
                           line1Vertex1->pos().y() - textItem->boundingRect().height());
        else
            return QPointF(line1Vertex2->pos().x() + line1Vertex2->crossSize().width(),
                           line1Vertex2->pos().y() - textItem->boundingRect().height());
    } else {
        if (angleCacheData <= 90.0)
            return QPointF(line1Vertex1->pos().x() - (line1Vertex1->crossSize().width() + textItem->boundingRect().width()),
                           line1Vertex1->y());
        else
            return QPointF(line1Vertex2->pos().x() - (line1Vertex2->crossSize().width() + textItem->boundingRect().width()),
                           line1Vertex2->y());
    }
}

//----------------------------------------------------------------
QPainterPath GraphicsCobbAngleItem::itemPath()
{
    QPainterPath cobbAngle;
    cobbAngle.moveTo(line1Vertex1->pos());
    cobbAngle.lineTo(line1Vertex2->pos());
    cobbAngle.moveTo(line2Vertex1->pos());
    cobbAngle.lineTo(line2Vertex2->pos());
    if (line2Vertex1->pos() != line2Vertex2->pos()) {
        angle();
        if (angleCacheData <= 90.00) {
            auxVertex->setPos(line1Vertex1->pos() + line2Vertex2->pos() - line2Vertex1->pos());
            cobbAngle.moveTo(line1Vertex1->pos());
            cobbAngle.lineTo(auxVertex->pos());
        } else {
            auxVertex->setPos(line1Vertex2->pos() + line2Vertex2->pos() - line2Vertex1->pos());
            cobbAngle.moveTo(line1Vertex2->pos());
            cobbAngle.lineTo(auxVertex->pos());
        }
    }
    return cobbAngle;
}

//----------------------------------------------------------------
void GraphicsCobbAngleItem::angle()
{
    QPointF v1 = line1Vertex2->pos() - line1Vertex1->pos();
    QPointF v2 = line2Vertex2->pos() - line2Vertex1->pos();
    double modulusProduct = sqrt((v1.x() * v1.x() + v1.y() * v1.y()) * (v2.x() * v2.x() + v2.y() * v2.y()));
    angleCacheData = acos(QPointF::dotProduct(v1, v2) / modulusProduct) * 180 / PI;
}
