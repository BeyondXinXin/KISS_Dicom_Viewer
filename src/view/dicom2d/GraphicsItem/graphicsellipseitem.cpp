#include "graphicsellipseitem.h"
#include "graphicscrossitem.h"
#include "graphicssimpletextitem.h"

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmimgle/dcmimage.h"
#include "dcmtk/dcmimgle/diutils.h"

#define TEXT_STYLE "Mean=%L1 SD=%L2\nMax=%L3 Min=%L4\nArea=%L5cm^2 (%L6 px)"

//----------------------------------------------------------------
GraphicsEllipseItem::GraphicsEllipseItem(QGraphicsItem *parent):
    area(0),
    updated(false),
    AbstractPathItem(parent) {
    init();
}

//----------------------------------------------------------------
void GraphicsEllipseItem::setActivePoint(const QPointF &point) {
    br->setPos(point);
    update();
}

//----------------------------------------------------------------
void GraphicsEllipseItem::init() {
    tl = new GraphicsCrossItem(this);
    br = new GraphicsCrossItem(this);
}

//----------------------------------------------------------------
void GraphicsEllipseItem::recalPixInfo(const DicomImage *dcmImage) {
    const DiPixel *pixel;
    if (dcmImage && (pixel = dcmImage->getInterData())) {
        int width = dcmImage->getWidth();
        int height = dcmImage->getHeight();
        QPointF topLeft = mapToParent(tl->pos() + QPointF(tl->crossSize().width() / 2, tl->crossSize().height() / 2));
        QPointF bottomRight = mapToParent(br->pos() + QPointF(br->crossSize().width() / 2, br->crossSize().height() / 2.0));
        QPointF center = QPointF((topLeft.x() + bottomRight.x()) / 2, (topLeft.y() + bottomRight.y()) / 2);
        double aa = (center.x() - topLeft.x()) * (center.x() - topLeft.x());
        double bb = (center.y() - topLeft.y()) * (center.y() - topLeft.y());
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
                    double tmp = bb * (y - center.y()) * (y - center.y());
                    for (int x = xStart; x < xEnd; ++x) {
                        if (aa * (x - center.x()) * (x - center.x()) + tmp < aa * bb) {
                            val = *((qint8 *)(pixel->getData()) + (y * width + x));
                            valCount += val;
                            maxVal = maxVal < val ? val : maxVal;
                            minVal = minVal < val ? minVal : val;
                            pixCount++;
                        }
                    }
                }
                break;
            case EPR_Uint8:
                for (int y = yStart; y < yEnd; ++y) {
                    double tmp = bb * (y - center.y()) * (y - center.y());
                    for (int x = xStart; x < xEnd; ++x) {
                        if (aa * (x - center.x()) * (x - center.x()) + tmp < aa * bb) {
                            val = *((quint8 *)(pixel->getData()) + (y * width + x));
                            valCount += val;
                            maxVal = maxVal < val ? val : maxVal;
                            minVal = minVal < val ? minVal : val;
                            pixCount++;
                        }
                    }
                }
                break;
            case EPR_Sint16:
                for (int y = yStart; y < yEnd; ++y) {
                    double tmp = bb * (y - center.y()) * (y - center.y());
                    for (int x = xStart; x < xEnd; ++x) {
                        if (aa * (x - center.x()) * (x - center.x()) + tmp < aa * bb) {
                            val = *((qint16 *)(pixel->getData()) + (y * width + x));
                            valCount += val;
                            maxVal = maxVal < val ? val : maxVal;
                            minVal = minVal < val ? minVal : val;
                            pixCount++;
                        }
                    }
                }
                break;
            case EPR_Uint16:
                for (int y = yStart; y < yEnd; ++y) {
                    double tmp = bb * (y - center.y()) * (y - center.y());
                    for (int x = xStart; x < xEnd; ++x) {
                        if (aa * (x - center.x()) * (x - center.x()) + tmp < aa * bb) {
                            val = *((quint16 *)(pixel->getData()) + (y * width + x));
                            valCount += val;
                            maxVal = maxVal < val ? val : maxVal;
                            minVal = minVal < val ? minVal : val;
                            pixCount++;
                        }
                    }
                }
                break;
            case EPR_Sint32:
                for (int y = yStart; y < yEnd; ++y) {
                    double tmp = bb * (y - center.y()) * (y - center.y());
                    for (int x = xStart; x < xEnd; ++x) {
                        if (aa * (x - center.x()) * (x - center.x()) + tmp < aa * bb) {
                            val = *((qint32 *)(pixel->getData()) + (y * width + x));
                            valCount += val;
                            maxVal = maxVal < val ? val : maxVal;
                            minVal = minVal < val ? minVal : val;
                            pixCount++;
                        }
                    }
                }
                break;
            case EPR_Uint32:
                for (int y = yStart; y < yEnd; ++y) {
                    double tmp = bb * (y - center.y()) * (y - center.y());
                    for (int x = xStart; x < xEnd; ++x) {
                        if (aa * (x - center.x()) * (x - center.x()) + tmp < aa * bb) {
                            val = *((quint32 *)(pixel->getData()) + (y * width + x));
                            valCount += val;
                            maxVal = maxVal < val ? val : maxVal;
                            minVal = minVal < val ? minVal : val;
                            pixCount++;
                        }
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
                    double tmp = bb * (y - center.y()) * (y - center.y());
                    for (int x = xStart; x < xEnd; ++x) {
                        if (aa * (x - center.x()) * (x - center.x()) + tmp < aa * bb) {
                            val = *((qint8 *)(pixel->getData()) + (y * width + x));
                            deviation += (val - mean) * (val - mean);
                        }
                    }
                }
                break;
            case EPR_Uint8:
                for (int y = yStart; y < yEnd; ++y) {
                    double tmp = bb * (y - center.y()) * (y - center.y());
                    for (int x = xStart; x < xEnd; ++x) {
                        if (aa * (x - center.x()) * (x - center.x()) + tmp < aa * bb) {
                            val = *((quint8 *)(pixel->getData()) + (y * width + x));
                            deviation += (val - mean) * (val - mean);
                        }
                    }
                }
                break;
            case EPR_Sint16:
                for (int y = yStart; y < yEnd; ++y) {
                    double tmp = bb * (y - center.y()) * (y - center.y());
                    for (int x = xStart; x < xEnd; ++x) {
                        if (aa * (x - center.x()) * (x - center.x()) + tmp < aa * bb) {
                            val = *((qint16 *)(pixel->getData()) + (y * width + x));
                            deviation += (val - mean) * (val - mean);
                        }
                    }
                }
                break;
            case EPR_Uint16:
                for (int y = yStart; y < yEnd; ++y) {
                    double tmp = bb * (y - center.y()) * (y - center.y());
                    for (int x = xStart; x < xEnd; ++x) {
                        if (aa * (x - center.x()) * (x - center.x()) + tmp < aa * bb) {
                            val = *((quint16 *)(pixel->getData()) + (y * width + x));
                            deviation += (val - mean) * (val - mean);
                        }
                    }
                }
                break;
            case EPR_Sint32:
                for (int y = yStart; y < yEnd; ++y) {
                    double tmp = bb * (y - center.y()) * (y - center.y());
                    for (int x = xStart; x < xEnd; ++x) {
                        if (aa * (x - center.x()) * (x - center.x()) + tmp < aa * bb) {
                            val = *((qint32 *)(pixel->getData()) + (y * width + x));
                            deviation += (val - mean) * (val - mean);
                        }
                    }
                }
                break;
            case EPR_Uint32:
                for (int y = yStart; y < yEnd; ++y) {
                    double tmp = bb * (y - center.y()) * (y - center.y());
                    for (int x = xStart; x < xEnd; ++x) {
                        if (aa * (x - center.x()) * (x - center.x()) + tmp < aa * bb) {
                            val = *((quint32 *)(pixel->getData()) + (y * width + x));
                            deviation += (val - mean) * (val - mean);
                        }
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
bool GraphicsEllipseItem::isModified() {
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
void GraphicsEllipseItem::updateTextItem() {
    textItem->setText(QString());
    if (xSpacing > 0 && ySpacing > 0) {
        QPointF p1 = mapToParent(tl->pos());
        QPointF p2 = mapToParent(br->pos());
        area = PI * (p2.x() - p1.x()) * (p2.y() - p1.y()) * xSpacing * ySpacing * 0.0025; //0.0025 = 0.01 / 4
        if (!updated) {
            textItem->setText(QObject::tr("%1 cm^2").arg(area, 0, 'f', 2));
        }
    }
}

//----------------------------------------------------------------
QPointF GraphicsEllipseItem::textItemPos() {
    return QPointF(br->x() + br->crossSize().width(), tl->y() + (br->y() - tl->y()) / 2);
}

//----------------------------------------------------------------
QPainterPath GraphicsEllipseItem::itemPath() {
    QPainterPath ellipse;
    ellipse.addEllipse(QRectF(tl->pos(), br->pos()));
    return ellipse;
}
