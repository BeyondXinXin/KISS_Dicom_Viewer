#include "graphicsarrowitem.h"




#define ARROW_SIZE (12.0 * zoomFactor)

//----------------------------------------------------------------
GraphicsArrowItem::GraphicsArrowItem(QGraphicsItem * parent)
  : AbstractPathItem(parent)
{
}

//----------------------------------------------------------------
void GraphicsArrowItem::setActivePoint(const QPointF & point)
{
    start = point;
}

//----------------------------------------------------------------
bool GraphicsArrowItem::isModified()
{
    return currentStage != Final;
}

//----------------------------------------------------------------
QPainterPath GraphicsArrowItem::itemPath()
{
    QPainterPath arrow;
    if (start == end) {
        return arrow;
    }
    QPointF auxVer1, auxVer2;
    QPointF mainVector = start - end;
    double A, mB, C;
    A = mainVector.x() * mainVector.x() + mainVector.y() * mainVector.y();
    if (A < 1) {
        return arrow;
    }
    double dotProduct = sqrt(A) * ARROW_SIZE * cos(PI / 6);
    mB = mainVector.y() * dotProduct * 2;
    C = dotProduct * dotProduct - ARROW_SIZE * ARROW_SIZE * mainVector.x() * mainVector.x();
    double delta = mB * mB - 4 * A * C;
    if (delta < 0) {
        delta = 0;
    } else {
        delta = sqrt(delta);
    }
    double v1Y = (mB + delta) / (2 * A);
    double v2Y = (mB - delta) / (2 * A);
    double v1X = sqrt(ARROW_SIZE * ARROW_SIZE - v1Y * v1Y);
    if (qIsNaN(v1X)) {
        v1X = 0;
    }
    double v2X = sqrt(ARROW_SIZE * ARROW_SIZE - v2Y * v2Y);
    if (qIsNaN(v2X)) {
        v2X = 0;
    }
    auxVer1.setY(v1Y + end.y());
    auxVer2.setY(v2Y + end.y());
    auxVer1.setX((dotProduct - mainVector.y() * v1Y) * mainVector.x() > 0 ? (end.x() + v1X) : (end.x() - v1X));
    auxVer2.setX((dotProduct - mainVector.y() * v2Y) * mainVector.x() > 0 ? (end.x() + v2X) : (end.x() - v2X));
    arrow.moveTo(start);
    arrow.lineTo(end);
    arrow.moveTo(auxVer1);
    arrow.lineTo(end);
    arrow.moveTo(auxVer2);
    arrow.lineTo(end);
    return arrow;
}
