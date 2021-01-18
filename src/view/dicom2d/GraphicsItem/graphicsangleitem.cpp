#include "graphicsangleitem.h"
#include "graphicscrossitem.h"
#include "graphicssimpletextitem.h"
#include "math.h"


//----------------------------------------------------------------
GraphicsAngleItem::GraphicsAngleItem(QGraphicsItem *parent):
    AbstractPathItem(parent) {
    init();
}

//----------------------------------------------------------------
void GraphicsAngleItem::init() {
    pointAngle = new GraphicsCrossItem(this);
    point1 = new GraphicsCrossItem(this);
    point2 = new GraphicsCrossItem(this);
    currentStage = First;
}

//----------------------------------------------------------------
void GraphicsAngleItem::setActivePoint(const QPointF &point) {
    switch (currentStage) {
        case First:
            point1->setPos(point);
            update();
            break;
        case Second:
            point2->setPos(point);
            update();
            break;
    }
}

//----------------------------------------------------------------
bool GraphicsAngleItem::isModified() {
    if (pointAngle->pos() != prevPAngle || point1->pos() != prevP1 || point2->pos() != prevP2) {
        prevPAngle = pointAngle->pos();
        prevP1 = point1->pos();
        prevP2 = point2->pos();
        return true;
    }
    return false;
}

//----------------------------------------------------------------
void GraphicsAngleItem::nextStage() {
    switch (currentStage) {
        case First:
            if (pointAngle->pos() != point1->pos()) {
                currentStage = Second;
            }
            break;
        case Second:
            if (point2->pos() != pointAngle->pos() &&
                    point2->pos() != point1->pos()) {
                currentStage = Final;
            }
            break;
    }
}

//----------------------------------------------------------------
void GraphicsAngleItem::updateTextItem() {
    if (currentStage == First || pointAngle->pos() == point2->pos()) {
        return;
    }
    QPointF vector1 = point1->pos() - pointAngle->pos();
    QPointF vector2 = point2->pos() - pointAngle->pos();
    double angle = acos(QPointF::dotProduct(vector1, vector2) /
                        sqrt((vector1.x() * vector1.x() + vector1.y() * vector1.y()) *
                             (vector2.x() * vector2.x() + vector2.y() * vector2.y())));
    double angle_Degree = angle * 180 / PI;
    textItem->setText(QObject::tr("%1 D").arg(angle_Degree, 0, 'f', 2));
}

//----------------------------------------------------------------
QPointF GraphicsAngleItem::textItemPos() {
    if (currentStage == First || pointAngle->pos() == point2->pos()) {
        return QPointF();
    }
    QPointF vector1 = point1->pos() - pointAngle->pos();
    QPointF vector2 = point2->pos() - pointAngle->pos();
    if ((vector1.x() < 0 || vector1.y() < 0) && (vector2.x() < 0 || vector2.y() < 0)) {
        return QPointF(pointAngle->pos().x() + pointAngle->crossSize().width(),
                       pointAngle->pos().y());
    } else if ((vector1.x() < 0 || vector1.y() > 0) && (vector2.x() < 0 || vector2.y() > 0)) {
        return QPointF(pointAngle->pos().x() + pointAngle->crossSize().width(),
                       pointAngle->pos().y() - textItem->boundingRect().height());
    } else {
        return QPointF(pointAngle->pos().x() -
                       (pointAngle->crossSize().width() + textItem->boundingRect().width()),
                       pointAngle->y());
    }
}

//----------------------------------------------------------------
QPainterPath GraphicsAngleItem::itemPath() {
    QPainterPath angle;
    angle.moveTo(pointAngle->pos());
    angle.lineTo(point1->pos());
    angle.moveTo(pointAngle->pos());
    angle.lineTo(point2->pos());
    return angle;
}
