#include "graphicstumoritem.h"
#include "graphicssimpletextitem.h"
#include "math.h"

#define TEXT_STYLE "Mean=%L1 SD=%L2\nMax=%L3 Min=%L4\nArea=%L5mm^2 (%L6 px)"

//----------------------------------------------------------------
GraphicsTumorItem::GraphicsTumorItem(QGraphicsItem *parent):
    modified(false),
    AbstractPathItem(parent) {
    setFlag(QGraphicsItem::ItemIsMovable, false);
    setFlag(QGraphicsItem::ItemIsSelectable, false);
}

//----------------------------------------------------------------
void GraphicsTumorItem::recalPixInfo(const short **vol, int width, int height, int slice, int index, ViewType type) {
    if (vol && m_starts.size() && m_starts.size() == m_ends.size()) {
        int maxVal = -32768;
        short minVal = 32767;
        short val;
        qint64 pixCount = 0;
        qint64 valTotal = 0;
        double mean, sd = 0;
        for (int i = 0; i < m_starts.size(); ++i) {
            if (m_starts.at(i).y() == m_ends.at(i).y()) {
                int y = m_starts.at(i).y();
                for (int j = m_starts.at(i).x(); j <= m_ends.at(i).x(); ++j) {
                    switch (type) {
                        case VT_XYPlane:
                            val = vol[index][y * width + j];
                            break;
                        case VT_XZPlane:
                            val = vol[y][index * width + j];
                            break;
                        case VT_YZPlane:
                            val = vol[y][j * width + index];
                            break;
                    }
                    pixCount++;
                    valTotal += val;
                    maxVal = maxVal < val ? val : maxVal;
                    minVal = minVal > val ? val : minVal;
                }
            }
        }
        mean = valTotal / double(pixCount);
        for (int i = 0; i < m_starts.size(); ++i) {
            if (m_starts.at(i).y() == m_ends.at(i).y()) {
                int y = m_starts.at(i).y();
                for (int j = m_starts.at(i).x(); j <= m_ends.at(i).x(); ++j) {
                    switch (type) {
                        case VT_XYPlane:
                            val = vol[index][y * width + j];
                            break;
                        case VT_XZPlane:
                            val = vol[y][index * width + j];
                            break;
                        case VT_YZPlane:
                            val = vol[y][j * width + index];
                            break;
                    }
                    sd += (val - mean) * (val - mean);
                }
            }
        }
        sd = sqrt(sd);
        textItem->setText(QString(TEXT_STYLE).arg(qint32(mean)).arg(sd, 0, 'f', 2)
                          .arg(maxVal).arg(minVal).arg(pixCount * xSpacing * ySpacing, 0, 'f', 2)
                          .arg(pixCount));
    } else {
        textItem->setText("");
    }
}

//----------------------------------------------------------------
QPointF GraphicsTumorItem::textItemPos() {
    if (m_ends.size()) {
        return QPointF(m_ends.last().x() + 1, m_ends.last().y() + 1);
    }
    return QPointF();
}

//----------------------------------------------------------------
QPainterPath GraphicsTumorItem::itemPath() {
    if (modified && (!m_starts.isEmpty())) {
        QPainterPath path;
        path.moveTo(m_starts.first() + QPoint(-1, -1));
        foreach (const QPoint &p, m_starts) {
            path.lineTo(p + QPoint(-1, 0));
        }
        path.lineTo(m_starts.last() + QPoint(-1, 1));
        path.lineTo(m_ends.last() + QPoint(1, 1));
        path.moveTo(m_starts.first() + QPoint(-1, -1));
        path.lineTo(m_ends.first() + QPoint(1, -1));
        foreach (const QPoint &p, m_ends) {
            path.lineTo(p + QPoint(1, 0));
        }
        path.lineTo(m_ends.last() + QPoint(1, 1));
        this->path = path;
        modified = false;
    }
    return path;
}
