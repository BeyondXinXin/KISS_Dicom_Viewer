#include "graphicsrectitem.h"
#include "graphicssimpletextitem.h"
#include "graphicscrossitem.h"

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmimgle/dcmimage.h"
#include "dcmtk/dcmimgle/diutils.h"

#define TEXT_STYLE "Mean=%L1 SD=%L2\nMax=%L3 Min=%L4\nArea=%L5cm^2 (%L6 px)"

//----------------------------------------------------------------
GraphicsRectItem::GraphicsRectItem(QGraphicsItem *parent):
    area(0),
    updated(false),
    AbstractPathItem(parent) {
    init();
}

//----------------------------------------------------------------
void GraphicsRectItem::init() {
    tl = new GraphicsCrossItem(this);
    br = new GraphicsCrossItem(this);
}

//----------------------------------------------------------------
void GraphicsRectItem::setActivePoint(const QPointF &point) {
    br->setPos(point);
    update();
}

//----------------------------------------------------------------
void GraphicsRectItem::recalPixInfo(const DicomImage *dcmImage) {
    const DiPixel *pixel;
    if (dcmImage && (pixel = dcmImage->getInterData())) {
        int width = dcmImage->getWidth();
        int height = dcmImage->getHeight();
        QPointF topLeft = mapToParent(tl->pos() + QPointF(tl->crossSize().width() / 2, tl->crossSize().height() / 2));
        QPointF bottomRight = mapToParent(br->pos() + QPointF(br->crossSize().width() / 2, br->crossSize().height() / 2.0));
        qint64 pixCount = 0;
        qint64 valCount = 0;
        qint64 maxVal = INT64_MIN;
        qint64 minVal = INT64_MAX;
        qint64 val;
        int xStart = topLeft.toPoint().x() < 0 ? 0 : topLeft.toPoint().x();
        int yStart = topLeft.toPoint().y() < 0 ? 0 : topLeft.toPoint().y();
        int xEnd = width < bottomRight.toPoint().x() ? width : bottomRight.toPoint().x();
        int yEnd = height < bottomRight.toPoint().y() ? height : bottomRight.toPoint().y();
        EP_Representation r = pixel->getRepresentation();
        switch (r) {
            case EPR_Sint8:
                for (int y = yStart; y < yEnd; ++y) {
                    for (int x = xStart; x < xEnd; ++x) {
                        val = *((qint8 *)(pixel->getData()) + (y * width + x));
                        valCount += val;
                        maxVal = maxVal < val ? val : maxVal;
                        minVal = minVal < val ? minVal : val;
                        pixCount++;
                    }
                }
                break;
            case EPR_Uint8:
                for (int y = yStart; y < yEnd; ++y) {
                    for (int x = xStart; x < xEnd; ++x) {
                        val = *((quint8 *)(pixel->getData()) + (y * width + x));
                        valCount += val;
                        maxVal = maxVal < val ? val : maxVal;
                        minVal = minVal < val ? minVal : val;
                        pixCount++;
                    }
                }
                break;
            case EPR_Sint16:
                for (int y = yStart; y < yEnd; ++y) {
                    for (int x = xStart; x < xEnd; ++x) {
                        val = *((qint16 *)(pixel->getData()) + (y * width + x));
                        valCount += val;
                        maxVal = maxVal < val ? val : maxVal;
                        minVal = minVal < val ? minVal : val;
                        pixCount++;
                    }
                }
                break;
            case EPR_Uint16:
                for (int y = yStart; y < yEnd; ++y) {
                    for (int x = xStart; x < xEnd; ++x) {
                        val = *((quint16 *)(pixel->getData()) + (y * width + x));
                        valCount += val;
                        maxVal = maxVal < val ? val : maxVal;
                        minVal = minVal < val ? minVal : val;
                        pixCount++;
                    }
                }
                break;
            case EPR_Sint32:
                for (int y = yStart; y < yEnd; ++y) {
                    for (int x = xStart; x < xEnd; ++x) {
                        val = *((qint32 *)(pixel->getData()) + (y * width + x));
                        valCount += val;
                        maxVal = maxVal < val ? val : maxVal;
                        minVal = minVal < val ? minVal : val;
                        pixCount++;
                    }
                }
                break;
            case EPR_Uint32:
                for (int y = yStart; y < yEnd; ++y) {
                    for (int x = xStart; x < xEnd; ++x) {
                        val = *((quint32 *)(pixel->getData()) + (y * width + x));
                        valCount += val;
                        maxVal = maxVal < val ? val : maxVal;
                        minVal = minVal < val ? minVal : val;
                        pixCount++;
                    }
                }
                break;
        }
        double mean = 0;
        if (pixCount > 0) {
            mean = double(valCount) / pixCount;
        }
        double deviation = 0;
        switch (r) {
            case EPR_Sint8:
                for (int y = yStart; y < yEnd; ++y) {
                    for (int x = xStart; x < xEnd; ++x) {
                        val = *((qint8 *)(pixel->getData()) + (y * width + x));
                        deviation += (val - mean) * (val - mean);
                    }
                }
                break;
            case EPR_Uint8:
                for (int y = yStart; y < yEnd; ++y) {
                    for (int x = xStart; x < xEnd; ++x) {
                        val = *((quint8 *)(pixel->getData()) + (y * width + x));
                        deviation += (val - mean) * (val - mean);
                    }
                }
                break;
            case EPR_Sint16:
                for (int y = yStart; y < yEnd; ++y) {
                    for (int x = xStart; x < xEnd; ++x) {
                        val = *((qint16 *)(pixel->getData()) + (y * width + x));
                        deviation += (val - mean) * (val - mean);
                    }
                }
                break;
            case EPR_Uint16:
                for (int y = yStart; y < yEnd; ++y) {
                    for (int x = xStart; x < xEnd; ++x) {
                        val = *((quint16 *)(pixel->getData()) + (y * width + x));
                        deviation += (val - mean) * (val - mean);
                    }
                }
                break;
            case EPR_Sint32:
                for (int y = yStart; y < yEnd; ++y) {
                    for (int x = xStart; x < xEnd; ++x) {
                        val = *((qint32 *)(pixel->getData()) + (y * width + x));
                        deviation += (val - mean) * (val - mean);
                    }
                }
                break;
            case EPR_Uint32:
                for (int y = yStart; y < yEnd; ++y) {
                    for (int x = xStart; x < xEnd; ++x) {
                        val = *((quint32 *)(pixel->getData()) + (y * width + x));
                        deviation += (val - mean) * (val - mean);
                    }
                }
                break;
        }
        double sd = sqrt(deviation);
        updated = true;
        textItem->setText(QString(TEXT_STYLE).arg(qint64(mean)).arg(sd, 0, 'f', 2)
                          .arg(maxVal).arg(minVal).arg(area, 0, 'f', 2).arg(pixCount));
    }
}

//----------------------------------------------------------------
bool GraphicsRectItem::isModified() {
    if (tl->x() >= br->x()) {
        tl->setX(prevTl.x());
        br->setX(prevBr.x());
    }
    if (tl->y() >= br->y()) {
        tl->setY(prevTl.y());
        br->setY(prevBr.y());
    }
    if (tl->pos() != prevTl || br->pos() != prevBr || pos() != prevThis) {
        prevTl = tl->pos();
        prevBr = br->pos();
        prevThis = pos();
        updated = false;
        return true;
    }
    return false;
}

//----------------------------------------------------------------
void GraphicsRectItem::updateTextItem() {
    textItem->setText(QString());
    if (xSpacing > 0 && ySpacing > 0) {
        QPointF p1 = mapToParent(tl->pos());
        QPointF p2 = mapToParent(br->pos());
        area = (p2.x() - p1.x()) * (p2.y() - p1.y()) * xSpacing * ySpacing * 0.01;
        if (!updated) {
            textItem->setText(QObject::tr("%1cm^2").arg(area, 0, 'f', 2));
        }
    }
}

//----------------------------------------------------------------
QPointF GraphicsRectItem::textItemPos() {
    return QPointF(br->x() + br->crossSize().width(), tl->y() + (br->y() - tl->y()) / 2);
}

//----------------------------------------------------------------
QPainterPath GraphicsRectItem::itemPath() {
    QPainterPath rect;
    rect.addRect(tl->x(), tl->y(), br->x() - tl->x(), br->y() - tl->y());
    return rect;
}
