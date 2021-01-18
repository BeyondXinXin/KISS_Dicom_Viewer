#ifndef ABSTRACTPATHITEM_H
#define ABSTRACTPATHITEM_H

#include <QGraphicsPathItem>
#include <QPen>
class GraphicsSimpleTextItem;
class DicomImage;

#define PI double(3.1415926)
#define MARGIN 2

class AbstractPathItem : public QGraphicsPathItem {
  public:
    enum Stage {
        First,
        Second,
        Third,
        Fouth,
        Fifth,
        Final,
    };

    explicit AbstractPathItem(QGraphicsItem *parent = nullptr);

    void setFont(const QFont &font);
    virtual void setActivePoint(const QPointF &point) = 0;
    AbstractPathItem::Stage getCurrentStage() {
        return currentStage;
    }
    virtual void nextStage() {}
    void setZoomFactor(const double &factor);
    void setPixelSpacing(const double &x, const double &y);
    virtual void recalPixInfo(const DicomImage * /*dcmImage*/) {}
    virtual void recalPixInfo(const short ** /*data*/) {}
    virtual bool pixInfoUpdated() const {
        return true;
    }

  protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr);

  private:
    void init();
    virtual void updateTextItem() {}
    virtual QPointF textItemPos() {
        return QPointF();
    }
    virtual QPainterPath itemPath() {
        return QPainterPath();
    }
    virtual bool isModified() {
        return false;
    }

  protected:
    GraphicsSimpleTextItem *textItem;
    Stage currentStage;
    double xSpacing;
    double ySpacing;
    double zoomFactor;

    bool hoverEnter;
    QPen stockPen;
};

#endif // ABSTRACTPATHITEM_H
