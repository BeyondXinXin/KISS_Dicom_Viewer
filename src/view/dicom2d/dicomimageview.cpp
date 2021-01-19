#include "dicomimageview.h"

#include <global/KissGlobal>
#include <engine/KissEngine>

#include "ImageData/seriesinstance.h"
#include "ImageData/imageinstance.h"
#include "GraphicsItem/abstractpathitem.h"
#include "GraphicsItem/graphicscrossitem.h"
#include "GraphicsItem/graphicsangleitem.h"
#include "GraphicsItem/graphicsarrowitem.h"
#include "GraphicsItem/graphicscobbangleitem.h"
#include "GraphicsItem/graphicsellipseitem.h"
#include "GraphicsItem/graphicslineitem.h"
#include "GraphicsItem/graphicsrectitem.h"
#include "GraphicsItem/graphicscliprectitem.h"
#include "GraphicsItem/graphicstextmarkdialog.h"
#include "GraphicsItem/graphicstextmarkitem.h"
#include "GraphicsItem/graphicstumoritem.h"

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dcfilefo.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcmetinf.h"
#include "dcmtk/dcmimgle/dcmimage.h"

//-------------------------------------------------------
DicomImageView::DicomImageView(
    ViewType type, SeriesInstance *series, QWidget *parent) :
    QGraphicsView(parent),
    m_scene_(new QGraphicsScene(this)),
    m_series_(nullptr),
    pixmap_item_(new QGraphicsPixmapItem),
    x_scalor_item_(new QGraphicsPathItem),
    y_scalor_item_(new QGraphicsPathItem),
    current_path_item_(nullptr),
    factor_(1.0),
    fix_factor_(1.0),
    mag_factor_(2),
    manual_zoom_(false),
    manual_pan_(false),
    hflip_(0),
    vflip_(0),
    rotate_angle_(0),
    m_vtype(type) {
    // QWidget
    QWidget::setFocusPolicy(Qt::StrongFocus);
    QWidget::setAcceptDrops(true);
    QWidget::setContextMenuPolicy(Qt::DefaultContextMenu);
    // QFrame
    QFrame::setFrameShape(QFrame::Box);
    QFrame::setFrameShadow(QFrame::Plain);
    QFrame::setLineWidth(1);
    // QAbstractScrollArea
    QAbstractScrollArea::setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    QAbstractScrollArea::setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    // QGraphicsView
    QGraphicsView::setResizeAnchor(QGraphicsView::AnchorViewCenter);
    QGraphicsView::setBackgroundBrush(QBrush(Qt::black));
    QGraphicsView::setAlignment(Qt::AlignCenter);
    QGraphicsView::setScene(m_scene_);
    // Self
    SetBorderHighlight(false);
    m_scene_->setSceneRect(-5000, -5000, 10000, 10000);
    sub_pixmapItem_ = new QGraphicsPixmapItem(pixmap_item_);
    sub_pixmapItem_->setTransformationMode(Qt::SmoothTransformation);
    sub_pixmapItem_->setVisible(false);
    mag_factor_item_ = new QGraphicsSimpleTextItem(sub_pixmapItem_);
    mag_factor_item_->setBrush(QColor(184, 200, 212));
    double f = QSettings().value(MAGNIFIER_FACTOR).toDouble();
    if (f > 1) {
        mag_factor_ = f;
    }
    QBitmap cur(32, 32), mask(32, 32);
    cur.fill(Qt::color0);
    mask.fill(Qt::color0);
    pixmap_item_->setTransformationMode(Qt::SmoothTransformation);
    pixmap_item_->setAcceptHoverEvents(true);
    x_scalor_item_->setPen(QPen(QColor(184, 200, 212)));
    y_scalor_item_->setPen(QPen(QColor(184, 200, 212)));
    m_scene_->addItem(pixmap_item_);
    m_scene_->addItem(x_scalor_item_);
    m_scene_->addItem(y_scalor_item_);
    SetSeriesInstance(series);
    mouse_left_state_.state = ROIWindow;
    mouse_mid_state_.state = ROIWindow;
    mouse_right_state_.state = Zoom;
}

//-------------------------------------------------------
DicomImageView::~DicomImageView() {
    QSettings().setValue(MAGNIFIER_FACTOR, mag_factor_);
    Slot_SeriesDelate();
}

//-------------------------------------------------------
void DicomImageView::SetAnnoTextFont(const QFont &font) {
    anno_font_ = font;
    foreach (GraphicsAnnoGroup *g, anno_grps_) {
        foreach (QGraphicsSimpleTextItem *t, g->items) {
            t->setFont(font);
        }
    }
    foreach (AbstractPathItem *item, item_list_) {
        item->setFont(font);
    }
    RepositionAuxItems();
}

//-------------------------------------------------------
void DicomImageView::SeriesInstanceAppend() {
    this->UpdateAnnotations();
}

//-------------------------------------------------------
void DicomImageView::SetSeriesInstance(SeriesInstance *series) {
    if (this->m_series_ && this->m_series_ != series) {
        disconnect(this->m_series_, &SeriesInstance::Signal_AboutToDelete,
                   this, &DicomImageView::Slot_SeriesDelate);
        this->m_series_->DelVolBuffer();
        qDeleteAll(item_list_);
        item_list_.clear();
    }
    FreeAnnoGroups();
    if (series) {
        connect(series, &SeriesInstance::Signal_AboutToDelete,
                this, &DicomImageView::Slot_SeriesDelate, Qt::UniqueConnection);
    }
    this->m_series_ = series;
    AllocAnnoGroups();
    emit Signal_StatusChanged(series != nullptr);
    UpdateScalors();
    UpdateAnnotations();
    Reset();
    m_scene_->update(m_scene_->sceneRect());
}

//-------------------------------------------------------
SeriesInstance *DicomImageView::GetSeriesInstance() const {
    return m_series_;
}

//-------------------------------------------------------
DicomImage *DicomImageView::getHardCopyGrayScaledImage() {
    if (m_series_ && m_series_->GetCurrImageInstance(m_vtype)) {
        ImageInstance *image = m_series_->GetCurrImageInstance(m_vtype);
        QPointF tl = pixmap_item_->mapFromScene(mapToScene(this->rect().topLeft()));
        QPointF br = pixmap_item_->mapFromScene(mapToScene(this->rect().bottomRight()));
        if (tl.x() > br.x()) {
            double temp = br.x();
            br.setX(tl.x());
            tl.setX(temp);
        }
        if (tl.y() > br.y()) {
            double temp = br.y();
            br.setY(tl.y());
            tl.setY(temp);
        }
        return image->CreateClippedImage(QRectF(tl, br).toRect(), rotate_angle_ % 360,
                                         hflip_, vflip_,
                                         image->GetPolarity() == EPP_Reverse);
    }
    return nullptr;
}

//-------------------------------------------------------
QImage DicomImageView::getTextLayerImage(const QSize &size) {
    QImage textlayer(size, QImage::Format_RGB32);
    if (m_series_ && m_series_->GetCurrImageInstance(m_vtype) && (!textlayer.isNull())) {
        textlayer.fill(Qt::black);
        QPainter painter(&textlayer);
        pixmap_item_->setOpacity(0);
        zoom_item_->setVisible(false);
        render(&painter);
        pixmap_item_->setOpacity(1);
        painter.end();
    }
    return textlayer;
}

//-------------------------------------------------------
QPixmap DicomImageView::getHardCopyPixmap() {
    QPixmap pixmap;
    DicomImage *hgImage = getHardCopyGrayScaledImage();
    if (hgImage && ImageInstance::Dcm2BmpHelper(*hgImage, pixmap)) {
        delete hgImage;
        QImage imageLayer = pixmap.toImage();
        int width = imageLayer.width();
        int height = imageLayer.height();
        QImage textLayer = getTextLayerImage(QSize(width, height));
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                if (textLayer.valid(x, y) && (textLayer.pixel(x, y) != 0xff000000) ) {
                    imageLayer.setPixel(x, y, 0xffffffff);
                }
            }
        }
        pixmap.convertFromImage(imageLayer);
    }
    return pixmap;
}

//-------------------------------------------------------
QImage DicomImageView::getRenderedImage() {
    QImage image(this->rect().size(), QImage::Format_ARGB32);
    QPainter painter(&image);
    render(&painter);
    painter.end();
    return image;
}

//-------------------------------------------------------
/**
 * @brief DicomImageView::GetCurrentDicomPath
 * 获取当前dicom路径
 * @return
 */
QString DicomImageView::GetImageFile() {
    if(HasSeries()) {
        return this->m_series_->GetImageFile();
    } else {
        return "";
    }
}

//-------------------------------------------------------
/**
 * @brief DicomImageView::GetImageNum
 * 获取图片数量
 * @return
 */
qint32 DicomImageView::GetImageNum() {
    if(HasSeries()) {
        return this->m_series_->GetFrameCount();
    } else {
        return 0;
    }
}

//-------------------------------------------------------
/**
 * @brief DicomImageView::SetBorderHighlight
 * 设置View边框是否高亮
 * @param yes
 */
void DicomImageView::SetBorderHighlight(bool yes) {
    QPalette p = palette();
    if (yes) {
        p.setColor(QPalette::Text, Qt::lightGray);
    } else {
        p.setColor(QPalette::Text, Qt::transparent);
    }
    setPalette(p);
}

//-------------------------------------------------------
/**
 * @brief DicomImageView::HasSeries
 * @return
 */
bool DicomImageView::HasSeries() {
    return m_series_ != nullptr;
}

//-------------------------------------------------------
/**
 * @brief DicomImageView::SetMagnifierPixmap
 * @param itemPos
 */
void DicomImageView::SetMagnifierPixmap(const QPointF &itemPos) {
    if (mag_factor_ > m_pref_.magnifier_max) {
        mag_factor_ = m_pref_.magnifier_max;
    }
    if (mag_factor_ < m_pref_.magnifier_min) {
        mag_factor_ = m_pref_.magnifier_min;
    }
    double srcWidth = (MAGNIFIER_SIZE) / (mag_factor_ * factor_);
    QRectF source(
        itemPos.x() - srcWidth / 2,
        itemPos.y() - srcWidth / 2,
        srcWidth,
        srcWidth);
    QPixmap subImage( source.size().toSize());
    QPainter painter(&subImage);
    painter.drawPixmap(QPoint(0, 0), pixmap_item_->pixmap(), source);
    painter.end();
    sub_pixmapItem_->setPos(
        itemPos.x() - MAGNIFIER_SIZE / (2 * factor_), itemPos.y() - MAGNIFIER_SIZE / (2 * factor_));
    sub_pixmapItem_->setPixmap(subImage.scaledToWidth(MAGNIFIER_SIZE, Qt::SmoothTransformation));
    sub_pixmapItem_->setScale(1 / factor_);
    sub_pixmapItem_->setVisible(true);
    mag_factor_item_->setText(QString("%1X").arg(mag_factor_));
    mag_factor_item_->setPos(sub_pixmapItem_->boundingRect().width() -
                             mag_factor_item_->boundingRect().width(), 0);
}

//-------------------------------------------------------
void DicomImageView::UpdateScalors() {
    double xSpacing, ySpacing;
    QPainterPath scalorX, scalorY;
    if (m_series_ && m_series_->GetPixSpacing(xSpacing, ySpacing, m_vtype)) {
        if (xSpacing > 0.000001 && ySpacing > 0.000001) {
            double psX = xSpacing;
            double psY = ySpacing;
            /*
            if (rotateAngle % 180) {
                psX = ySpacing;
                psY = xSpacing;
            }
            */
            fix_factor_ = psY / psX;
            psY = psX;
            double widthR = 100 / psX;
            double heightR = 100 / psY;
            double stepX = widthR / 10;
            double tickX = stepX / 4;
            double stepY = heightR / 10;
            double tickY = stepY / 4;
            scalorX.moveTo(0, 0);
            scalorX.lineTo(widthR, 0);
            scalorY.moveTo(0, 0);
            scalorY.lineTo(0, heightR);
            for (int i = 0; i <= 10; ++i) {
                scalorX.moveTo(i * stepX, 0);
                scalorX.lineTo(i * stepX, -tickX);
                scalorY.moveTo(0, i * stepY);
                scalorY.lineTo(tickY, i * stepY);
            }
        }
    }
    x_scalor_item_->setPath(scalorX);
    y_scalor_item_->setPath(scalorY);
}

//-------------------------------------------------------
/**
 * @brief DicomImageView::AllocAnnoGroups
 * 分配描述 和 参数
 */
void DicomImageView::AllocAnnoGroups() {
    if (!m_series_) {
        return;
    }
    const ModalityProp *p = ModalityProperty::Instance()->getModalityProp(
                                m_series_->GetTagKeyValue(DCM_Modality));
    if (!p) {
        return;
    }
    m_pref_ = p->pref;
    foreach (const AnnoItemGroup *aig, p->groups) {
        GraphicsAnnoGroup *gag = new GraphicsAnnoGroup;
        anno_grps_ << gag;
        gag->pos = aig->pos;
        foreach (const AnnoItem *ai, aig->items) {
            Q_UNUSED(ai);
            QGraphicsSimpleTextItem *i = new QGraphicsSimpleTextItem;
            m_scene_->addItem(i);
            gag->items << i;
        }
    }
}

//-------------------------------------------------------
/**
 * @brief DicomImageView::UpdateAnnotations
 * 更新 描述文字
 */
void DicomImageView::UpdateAnnotations() {
    if (!(m_series_)) {
        FreeAnnoGroups();
        return;
    }
    const ModalityProp *p = ModalityProperty::Instance()->getModalityProp(
                                m_series_->GetTagKeyValue(DCM_Modality));
    if (!p) {
        return;
    }
    foreach (const GraphicsAnnoGroup *gag, anno_grps_) {
        foreach (const AnnoItemGroup *aig, p->groups) {
            if (gag->pos == aig->pos && gag->items.size() == aig->items.size()) {
                for (int i = 0; i < aig->items.size(); ++i) {
                    const AnnoItem *ai = aig->items.at(i);
                    QGraphicsSimpleTextItem *gi = gag->items.at(i);
                    gi->setBrush(QColor(184, 200, 212));
                    gi->setFont(anno_font_);
                    QString t = ai->text;
                    bool reserved = false;
                    foreach (const DcmTagKey &k, ai->keys) {
                        if (k.getGroup() != Dcm_AF_Group) {
#if HideNmae==1
                            if( k.getBaseTag() == DcmTagKey(0x0010, 0x0010)) {
                                t = t.arg("hide name");
                            } else if(k.getBaseTag() == DcmTagKey(0x0008, 0x0080)) {
                                t = t.arg("hide institution");
                            } else {
                                t = t.arg(m_series_->GetTagKeyValue(k, m_vtype));
                            }
#else
                            t = t.arg(m_series_->GetTagKeyValue(k, m_vtype));
#endif
                        } else if (k == DCM_AF_CursorX ||
                                   k == DCM_AF_CursorY ||
                                   k == DCM_AF_PixelValue) {
                            pos_value_item_ = gi;
                            pos_value_item_->setBrush(Qt::darkYellow);
                            pos_text_pattern_ = t;
                            reserved = true;
                            break;
                        } else if (k == DCM_AF_ZoomFactor) {
                            zoom_item_ = gi;
                            zoom_text_pattern_ = t;
                            reserved = true;
                            break;
                        } else if (k == DCM_AF_WindowCenter || k == DCM_AF_WindowWidth) {
                            window_item_ = gi;
                            window_text_pattern_ = t;
                            reserved = true;
                            break;
                        }
                    }
                    if (!reserved) {
                        gi->setText(t);
                    }
                }
                break;
            }
        }
    }
}

//-------------------------------------------------------
/**
 * @brief DicomImageView::FreeAnnoGroups
 * 清空描述
 */
void DicomImageView::FreeAnnoGroups() {
    qDeleteAll(anno_grps_);
    anno_grps_.clear();
    m_pref_ = ModalityPref();
    pos_value_item_ = nullptr;
    window_item_ = nullptr;
    zoom_item_ = nullptr;
}

//-------------------------------------------------------
/**
 * @brief DicomImageView::MousePressHandle
 * @param event
 * @param state
 * @param type
 */
void DicomImageView::MousePressHandle(
    QMouseEvent *event,
    const CurrentState &state,
    const DicomImageView::DrawingType &type) {
    QPointF sp = mapToScene(event->pos());
    QPointF ip = pixmap_item_->mapFromScene(sp);
    setDragMode(QGraphicsView::NoDrag);
    switch (state) {
        case None:
            break;
        case ROIWindow:
            setDragMode(QGraphicsView::RubberBandDrag);
            break;
        case Drawing:
            if (current_path_item_ == nullptr) {
                switch (type) {
                    case DrawLine:
                        current_path_item_ = new GraphicsLineItem(pixmap_item_);
                        break;
                    case DrawAngle:
                        current_path_item_ = new GraphicsAngleItem(pixmap_item_);
                        break;
                    case DrawCobbAngle:
                        current_path_item_ = new GraphicsCobbAngleItem(pixmap_item_);
                        break;
                    case DrawRect:
                        current_path_item_ = new GraphicsRectItem(pixmap_item_);
                        break;
                    case DrawEllipse:
                        current_path_item_ = new GraphicsEllipseItem(pixmap_item_);
                        break;
                    case DrawTextMark:
                        break;
                    case DrawArrow:
                        current_path_item_ = new GraphicsArrowItem(pixmap_item_);
                        break;
                    default:
                        break;
                }
                if (current_path_item_ != nullptr) {
                    current_path_item_->setPos(ip);
                    current_path_item_->setZoomFactor(factor_);
                    current_path_item_->setFont(anno_font_);
                    current_path_item_->grabMouse();
                    double xSpacing, ySpacing;
                    m_series_->GetPixSpacing(xSpacing, ySpacing, m_vtype);
                    current_path_item_->setPixelSpacing(xSpacing, ySpacing);
                    current_path_item_->setFlag(QGraphicsItem::ItemIgnoresParentOpacity);
                }
            }
            break;
        case Zoom:
            manual_zoom_ = true;
            break;
        case Pan:
            manual_pan_ = true;
            break;
        case Magnifier:
            SetMagnifierPixmap(ip);
            break;
        default:
            break;
    }
}

//-------------------------------------------------------
/**
 * @brief DicomImageView::MouseMoveHandle
 * @param event
 * @param state
 */
void DicomImageView::MouseMoveHandle(
    QMouseEvent *event, const CurrentState &state) {
    //
    if (event->modifiers()&Qt::ControlModifier) { // Ctrl
        setDragMode(QGraphicsView::NoDrag);
        if ((event->buttons() & Qt::LeftButton)) {
            manual_pan_ = true;
            m_scene_->clearSelection();
            if (m_scene_->mouseGrabberItem()) {
                m_scene_->mouseGrabberItem()->ungrabMouse();
            }
            QPoint deltaPos = event->pos() - prev_mouse_pos_;
            pixmap_item_->setPos(pixmap_item_->pos() + deltaPos);
        } else {
        }
        return;
    }
    //
    QPointF sp = mapToScene(event->pos());// 鼠标坐标(映射到场景)
    QPointF ip = pixmap_item_->mapFromScene(sp);// 图片坐标
    switch (state) {
        case None: {
                break;
            }
        case AdjustWL: {
                setDragMode(QGraphicsView::NoDrag);
                QPoint delta = event->pos() - prev_mouse_pos_;
                m_series_->SetWindowDelta(-delta.y()*m_pref_.adjust_factor,
                                          delta.x()*m_pref_.adjust_factor);
                RefreshPixmap();
                break;
            }
        case Zoom: {
                setDragMode(QGraphicsView::NoDrag);
                m_scene_->clearSelection();
                if (m_scene_->mouseGrabberItem()) {
                    m_scene_->mouseGrabberItem()->ungrabMouse();
                }
                int delta = prev_mouse_pos_.y() - event->pos().y();
                if (delta > 0) {
                    factor_ *= (1 + delta * m_pref_.zoom_factor);
                } else {
                    factor_ /= (1 - delta * m_pref_.zoom_factor);
                }
                ResizePixmapItem();
                RepositionAuxItems();
                break;
            }
        case Pan: {
                setDragMode(QGraphicsView::NoDrag);
                m_scene_->clearSelection();
                if (m_scene_->mouseGrabberItem()) {
                    m_scene_->mouseGrabberItem()->ungrabMouse();
                }
                QPoint deltaPos = event->pos() - prev_mouse_pos_;
                pixmap_item_->setPos(pixmap_item_->pos() + deltaPos);
                break;
            }
        case Slicing: {
                setDragMode(QGraphicsView::NoDrag);
                int frames = event->pos().y() - prev_mouse_pos_.y();
                if (frames > 0) {
                    m_series_->NextFrame(m_vtype);
                } else {
                    m_series_->PrevFrame(m_vtype);
                }
                RefreshPixmap();
                UpdateAnnotations();
                break;
            }
        case Magnifier: {
                setDragMode(QGraphicsView::NoDrag);
                m_scene_->clearSelection();
                if (m_scene_->mouseGrabberItem()) {
                    m_scene_->mouseGrabberItem()->ungrabMouse();
                }
                SetMagnifierPixmap(ip);
                break;
            }
        case ROIWindow: {
                QGraphicsView::mouseMoveEvent(event);
                break;
            }
        case Drawing: {
                setDragMode(QGraphicsView::NoDrag);
                if (mouse_left_state_.type == DrawTextMark) {
                    current_path_item_->setPos(ip);
                } else {
                    current_path_item_->setActivePoint(
                        current_path_item_->mapFromParent(ip));
                    m_scene_->update();
                }
                break;
            }
    }
}

//-------------------------------------------------------
/**
 * @brief DicomImageView::MouseReleaseHandle
 * @param state
 */
void DicomImageView::MouseReleaseHandle(QMouseEvent *, const CurrentState &state) {
    switch (state) {
        case None: {
                break;
            }
        case AdjustWL: {
                break;
            }
        case Zoom: {
                break;
            }
        case Pan: {
                break;
            }
        case Slicing: {
                break;
            }
        case Magnifier: {
                sub_pixmapItem_->setVisible(false);
                break;
            }
        case ROIWindow: {
                if (m_scene_->selectedItems().size() == 0) {
                    if (rubberBandRect().isValid()) {
                        m_series_->SetRoiWindow(
                            pixmap_item_->mapFromScene(
                                mapToScene(rubberBandRect())).boundingRect());
                        RefreshPixmap();
                    }
                    foreach (AbstractPathItem *it, item_list_) {
                        if (!it->pixInfoUpdated()) {
                            it->recalPixInfo(
                                m_series_->GetCurrImageInstance(m_vtype)->GetDcmImage());
                            m_scene_->update();
                        }
                    }
                }
                break;
            }
        case Drawing: {
                if (current_path_item_ != nullptr) {
                    if (mouse_left_state_.type == DrawTextMark) {
                        //current_path_item_->setPos(ip);
                        current_path_item_->ungrabMouse();
                        item_list_.append(current_path_item_);
                        current_path_item_ = nullptr;
                        mouse_left_state_.state = None;
                    } else {
                        current_path_item_->nextStage();
                        if (current_path_item_->getCurrentStage() ==
                                AbstractPathItem::Final) {
                            current_path_item_->ungrabMouse();
                            item_list_.append(current_path_item_);
                            current_path_item_->recalPixInfo(
                                m_series_->GetCurrImageInstance(m_vtype)->GetDcmImage());
                            current_path_item_ = nullptr;
                        }
                    }
                }
                break;
            }
    }
}

//-------------------------------------------------------
/**
 * @brief DicomImageView::PosValueShow
 * 鼠标位置显示
 * @param event
 */
void DicomImageView::PosValueShow(QMouseEvent *event) {
    QPointF sp = mapToScene(event->pos());// 鼠标坐标(映射到场景)
    QPointF ip = pixmap_item_->mapFromScene(sp);// 图片坐标
    if (pixmap_item_->contains(ip)) {
        QPoint pos = ip.toPoint();
        double v = m_series_->GetPixelValue(pos.x(), pos.y(), m_vtype);
        if(v < 10000.0 && v > -3000.0) {
            pos_value_item_->setText(pos_text_pattern_
                                     .arg(pos.x())
                                     .arg(pos.y())
                                     .arg(QString::number(v, 'f', 2)));
        } else {
            pos_value_item_->setText(pos_text_pattern_
                                     .arg(pos.x())
                                     .arg(pos.y())
                                     .arg("-"));
        }
    } else {
        pos_value_item_->setText("");
    }
}

//-------------------------------------------------------
/**
 * @brief DicomImageView::RepositionAuxItems
 * 描述定位
 */
void DicomImageView::RepositionAuxItems() {
    QPointF sceneTL = mapToScene(rect().topLeft());
    QPointF sceneBR = mapToScene(rect().bottomRight());
    foreach (const GraphicsAnnoGroup *g, anno_grps_) {
        int h = 0;
        if (g->pos == "topleft") {
            foreach (QGraphicsSimpleTextItem *t, g->items) {
                if (t->text().isEmpty() && t != pos_value_item_) {
                    continue;
                }
                t->setPos(sceneTL.x(), sceneTL.y() + h);
                h += QFontMetrics(t->font()).lineSpacing();
            }
        } else if (g->pos == "topright") {
            foreach (QGraphicsSimpleTextItem *t, g->items) {
                if (t->text().isEmpty() && t != pos_value_item_) {
                    continue;
                }
                t->setPos(sceneBR.x() -
                          QFontMetrics(t->font()).boundingRect(t->text()).width()
                          - 2, sceneTL.y() + h);
                h += QFontMetrics(t->font()).lineSpacing();
            }
        } else if (g->pos == "bottomleft") {
            for (int i = g->items.size(); i > 0; --i) {
                QGraphicsSimpleTextItem *t = g->items.at(i - 1);
                //if (t->text().isEmpty() && t != posValue) continue;
                t->setPos(sceneTL.x(), sceneBR.y()
                          - QFontMetrics(t->font()).lineSpacing() - h);
                h += QFontMetrics(t->font()).lineSpacing();
            }
        } else if (g->pos == "bottomright") {
            for (int i = g->items.size(); i > 0; --i) {
                QGraphicsSimpleTextItem *t = g->items.at(i - 1);
                if (t->text().isEmpty() && t != pos_value_item_) {
                    continue;
                }
                t->setPos(sceneBR.x() -
                          QFontMetrics(t->font()).boundingRect(t->text()).width() - 2,
                          sceneBR.y()
                          - QFontMetrics(t->font()).lineSpacing() - h);
                h += QFontMetrics(t->font()).lineSpacing();
            }
        }
    }
    QSizeF xRect = x_scalor_item_->boundingRect().size() * factor_;
    x_scalor_item_->setPos(sceneTL.x() + ((sceneBR.x() - sceneTL.x()) - xRect.width()) / 2,
                           sceneBR.y() - xRect.height());
    QSizeF yRect = y_scalor_item_->boundingRect().size() * factor_;
    y_scalor_item_->resetTransform();
    y_scalor_item_->setPos(sceneTL.x() + yRect.width(),
                           sceneTL.y() + ((sceneBR.y() - sceneTL.y()) - yRect.height()) / 2);
    if (factor_ > 2) {
        x_scalor_item_->setVisible(false);
        y_scalor_item_->setVisible(false);
    } else {
        x_scalor_item_->setVisible(true);
        y_scalor_item_->setVisible(true);
    }
}

//-------------------------------------------------------
/**
 * @brief DicomImageView::ResizePixmapItem
 * 图片缩放
 */
void DicomImageView::ResizePixmapItem() {
    if (!manual_zoom_) {
        QRectF pixmapRect = pixmap_item_->boundingRect();
        QRectF viewRect = this->rect();
        if (pixmapRect.width() > 0 && pixmapRect.height() > 0) {
            if (pixmapRect.width()*viewRect.height() <
                    pixmapRect.height()*fix_factor_ * viewRect.width()) {
                factor_ = viewRect.height() / (pixmapRect.height() * fix_factor_);
            } else {
                factor_ = viewRect.width() / pixmapRect.width();
            }
        }
    }
    if (factor_ > m_pref_.zoom_max) {
        factor_ = m_pref_.zoom_max;
    }
    if (factor_ < m_pref_.zoom_min) {
        factor_ = m_pref_.zoom_min;
    }
    if (!pixmap_item_->pixmap().isNull()) {
        pixmap_item_->setTransform(QTransform(factor_, 0, 0, factor_ * fix_factor_, 0, 0));
        if (!manual_pan_) {
            centerOn(pixmap_item_);
        }
        if (zoom_item_) {
            zoom_item_->setText(zoom_text_pattern_.arg(factor_ * 100, 0, 'f', 2));
        }
        x_scalor_item_->setScale(factor_);
        y_scalor_item_->setScale(factor_);
        QPen pen = x_scalor_item_->pen();
        pen.setWidthF(1 / factor_);
        x_scalor_item_->setPen(pen);
        y_scalor_item_->setPen(pen);
        foreach (QGraphicsItem *item, item_list_) {
            static_cast<AbstractPathItem *>(item)->setZoomFactor(factor_);
        }
    } else {
        if (zoom_item_) {
            zoom_item_->setText("");
        }
    }
}

//-------------------------------------------------------
/**
 * @brief DicomImageView::mouseDoubleClickEvent
 * 双击
 * @param event
 */
void DicomImageView::mouseDoubleClickEvent(QMouseEvent *event) {
    emit Singal_viewDoubleclicked(this);
    QGraphicsView::mouseDoubleClickEvent(event);
}

//-------------------------------------------------------
/**
 * @brief DicomImageView::mousePressEvent
 * 单机
 * @param event
 */
void DicomImageView::mousePressEvent(QMouseEvent *event) {
    emit Signal_ViewClicked(this);
    if (!m_series_) { // 没有 Series Instance
        QGraphicsView::mousePressEvent(event);
        return;
    }
    // 按键判断
    if (event->button() == Qt::RightButton) { // 鼠标右键
        MousePressHandle(event, mouse_right_state_.state, mouse_right_state_.type);
    } else if (event->button() == Qt::MiddleButton) { // 鼠标中键
        MousePressHandle(event, mouse_mid_state_.state, mouse_mid_state_.type);
    } else if (event->button() == Qt::LeftButton) { // 鼠标左键
        MousePressHandle(event, mouse_left_state_.state, mouse_left_state_.type);
    }
    prev_mouse_pos_ = event->pos();
    QGraphicsView::mousePressEvent(event);
}

//-------------------------------------------------------
/**
 * @brief DicomImageView::mouseMoveEvent
 * 移动
 * @param event
 */
void DicomImageView::mouseMoveEvent(QMouseEvent *event) {
    if (!m_series_) { // 没有 Series Instance
        QGraphicsView::mouseMoveEvent(event);
        return;
    }
    // 显示 当前位置和灰度
    if (pos_value_item_) {
        PosValueShow(event);
    }
    // 按键判断
    if (event->buttons() & Qt::RightButton) { // 鼠标右键拖动
        MouseMoveHandle(event, mouse_right_state_.state);
    } else if (event->buttons() & Qt::MiddleButton) { // 鼠标中键拖动
        MouseMoveHandle(event, mouse_mid_state_.state);
    } else if (event->buttons() & Qt::LeftButton) { // 鼠标左键拖动
        MouseMoveHandle(event, mouse_left_state_.state);
    } else {
        QGraphicsView::mouseMoveEvent(event);
    }
    prev_mouse_pos_ = event->pos();
}

//-------------------------------------------------------
/**
 * @brief DicomImageView::mouseReleaseEvent
 * 松开
 * @param event
 */
void DicomImageView::mouseReleaseEvent(QMouseEvent *event) {
    if (!m_series_) { // 没有 Series Instance
        QGraphicsView::mouseReleaseEvent(event);
        return;
    }
    // 按键判断
    if (event->button() == Qt::RightButton) { // 鼠标右键
        MouseReleaseHandle(event, mouse_right_state_.state);
    } else if (event->button() == Qt::MiddleButton) { // 鼠标中键
        MouseReleaseHandle(event, mouse_mid_state_.state);
    } else if (event->button() == Qt::LeftButton) { // 鼠标左键
        MouseReleaseHandle(event, mouse_left_state_.state);
    }
    QGraphicsView::mouseReleaseEvent(event);
}

//-------------------------------------------------------
/**
 * @brief DicomImageView::wheelEvent
 * 滚轮
 * @param e
 */
void DicomImageView::wheelEvent(QWheelEvent *e) {
    QPoint delta = e->angleDelta();
    if (m_series_ == nullptr) {
        return;
    }
    if ((e->buttons() & Qt::LeftButton) && (mouse_left_state_.state == Magnifier)) {
        if (delta.y() > 0) {
            mag_factor_ += m_pref_.magnifier_inc;
        } else {
            mag_factor_ -= m_pref_.magnifier_inc;
        }
        QPointF sp = mapToScene(e->pos());
        SetMagnifierPixmap(pixmap_item_->mapFromScene(sp));
    } else if (e->modifiers() & Qt::ControlModifier) {
        manual_zoom_ = true;
        if (delta.y() > 0) {
            factor_ *= (1 + delta.y() * m_pref_.zoom_factor);
        } else {
            factor_ /= (1 - delta.y() * m_pref_.zoom_factor);
        }
        ResizePixmapItem();
        RepositionAuxItems();
    } else {
        if (delta.y() > 0) {
            PrevFrame();
        } else if (delta.y() < 0) {
            NextFrame();
        }
    }
}

//-------------------------------------------------------
void DicomImageView::NextFrame() {
    m_series_->NextFrame(m_vtype);
    RefreshPixmap();
    UpdateAnnotations();
}

//-------------------------------------------------------
void DicomImageView::PrevFrame() {
    m_series_->PrevFrame(m_vtype);
    RefreshPixmap();
    UpdateAnnotations();
}

//-------------------------------------------------------
/**
 * @brief DicomImageView::leaveEvent
 * 鼠标移除
 * @param event
 */
void DicomImageView::leaveEvent(QEvent *event) {
    if (pos_value_item_) {
        pos_value_item_->setText("");
    }
    QGraphicsView::leaveEvent(event);
}

//-------------------------------------------------------
/**
 * @brief DicomImageView::keyPressEvent
 * 键盘事件
 * @param event
 */
void DicomImageView::keyPressEvent(QKeyEvent *e) {
    switch (e->key()) {
        case Qt::Key_Delete:
            RemoveCurrentDrawingItem();
            break;
        case Qt::Key_Up:
            PrevFrame();
            break;
        case Qt::Key_Down:
            NextFrame();
            break;
        case Qt::Key_Escape:
            SetOperation(None);
            break;
        case Qt::Key_Control:
            if (mouse_left_state_.state == None) {
            }
            break;
        case Qt::Key_Right:
        case Qt::Key_Left:
            break;
        default:
            QGraphicsView::keyPressEvent(e);
    }
}

//-------------------------------------------------------
/**
 * @brief DicomImageView::keyReleaseEvent
 * 键盘松开
 * @param event
 */
void DicomImageView::keyReleaseEvent(QKeyEvent *e) {
    if (e->key() == Qt::Key_Control &&
            mouse_left_state_.state == None) {
    } else {
        QGraphicsView::keyReleaseEvent(e);
    }
}

//-------------------------------------------------------
/**
 * @brief DicomImageView::resizeEvent
 * 界面重新调整大小
 * @param event
 */
void DicomImageView::resizeEvent(QResizeEvent *event) {
    QGraphicsView::resizeEvent(event);
    if (!manual_zoom_) {
        this->SetOperation(FillViewport);
    }
    ResizePixmapItem();
    RepositionAuxItems();
}

//-------------------------------------------------------
/**
 * @brief DicomImageView::dragEnterEvent
 * 拖拽进入
 * @param e
 */
void DicomImageView::dragEnterEvent(QDragEnterEvent *e) {
    if (e->mimeData()->hasFormat("text/plain")) {
        e->acceptProposedAction();
    }
}

//-------------------------------------------------------
/**
 * @brief DicomImageView::dragMoveEvent
 * 拖拽移动
 * @param e
 */
void DicomImageView::dragMoveEvent(QDragMoveEvent *e) {
    if (e->mimeData()->hasFormat("text/plain")) {
        e->acceptProposedAction();
    }
}

//-------------------------------------------------------
/**
 * @brief DicomImageView::dropEvent
 * 拖拽松开
 * @param e
 */
void DicomImageView::dropEvent(QDropEvent *e) {
    if (e->mimeData()->hasFormat("text/plain")) {
        e->acceptProposedAction();
        SeriesInstance *s = qobject_cast<SeriesInstance *>(
                                (QObject *)(e->mimeData()->text().toULongLong()));
        if (s) {
            SetSeriesInstance(s);
            emit Signal_ViewClicked(this);
        }
        QGraphicsView::dropEvent(e);
    }
}

//-------------------------------------------------------
/**
 * @brief DicomImageView::dragLeaveEvent
 * 离开事件
 * @param e
 */
void DicomImageView::dragLeaveEvent(QDragLeaveEvent *) {
    return;
}

//-------------------------------------------------------
/**
 * @brief DicomImageView::RefreshPixmap
 * 刷新图片
 */
void DicomImageView::RefreshPixmap() {
    QPixmap pixmap;
    if (m_series_) {
        m_series_->GetPixmap(pixmap, m_vtype);
        pixmap_item_->setPixmap(pixmap);
        pixmap_item_->setTransformOriginPoint(pixmap_item_->boundingRect().center());
        double center, width;
        m_series_->GetWindow(center, width);
        if (window_item_) {
            window_item_->setText(window_text_pattern_
                                  .arg(static_cast<qint32>(center))
                                  .arg(static_cast<qint32>(width)));
        }
    } else {
        pixmap_item_->setPixmap(pixmap);
        if (window_item_) {
            window_item_->setText("");
        }
        if (pos_value_item_) {
            pos_value_item_->setText("");
        }
    }
}

//-------------------------------------------------------
void DicomImageView::SetOperation(
    const DicomImageView::ZoomOperation &operation) {
    bool b = manual_zoom_;
    manual_zoom_ = true;
    switch (operation) {
        case FillViewport: {
                manual_zoom_ = false;
                ResizePixmapItem();
                centerOn(pixmap_item_);
                RepositionAuxItems();
                manual_zoom_ = b;
                return;
            }
        case Zoom100: {
                factor_ = 1;
                break;
            }
        case Zoom200: {
                factor_ = 2;
                break;
            }
        case Zoom400: {
                factor_ = 4;
                break;
            }
        case ZoomIn: {
                factor_ *= 1.2;
                break;
            }
        case Zoomout: {
                factor_ *= 0.8;
                break;
            }
    }
    ResizePixmapItem();
    RepositionAuxItems();
    manual_zoom_ = b;
}

//-------------------------------------------------------
void DicomImageView::SetOperation(
    const DicomImageView::RoateFlipOperation &operation) {
    if (!m_series_) {
        return;
    }
    switch (operation) {
        case HFlip: {
                QRectF pRect = pixmap_item_->boundingRect();
                pixmap_item_->setTransform(QTransform(-1, 0, 0, 1, pRect.width(), 0), true);
                hflip_ = !hflip_;
                break;
            }
        case VFlip: {
                QRectF pRect = pixmap_item_->boundingRect();
                pixmap_item_->setTransform(QTransform(1, 0, 0, -1, 0, pRect.height()), true);
                vflip_ = !vflip_;
                break;
            }
        case ClearFlip: {
                if (hflip_) {
                    SetOperation(HFlip);
                }
                if (vflip_) {
                    SetOperation(VFlip);
                }
                break;
            }
        case RoateCCW: {
                rotate_angle_ += 270;
                pixmap_item_->setRotation(rotate_angle_);
                break;
            }
        case RoateCW: {
                rotate_angle_ += 90;
                pixmap_item_->setRotation(rotate_angle_);
                break;
            }
        case ClearRoate: {
                pixmap_item_->setRotation(0);
                rotate_angle_ = 0;
                break;
            }
    }
}

//-------------------------------------------------------
void DicomImageView::SetOperation(const DicomImageView::DrawingType &operation) {
    if (!m_series_) {
        return;
    }
    switch (operation) {
        case DrawLine: {
                mouse_left_state_.state = Drawing;
                mouse_left_state_.type = DrawLine;
                break;
            }
        case DrawAngle: {
                mouse_left_state_.state = Drawing;
                mouse_left_state_.type = DrawAngle;
                break;
            }
        case DrawCobbAngle: {
                mouse_left_state_.state = Drawing;
                mouse_left_state_.type = DrawCobbAngle;
                break;
            }
        case DrawRect: {
                mouse_left_state_.state = Drawing;
                mouse_left_state_.type = DrawRect;
                break;
            }
        case DrawEllipse: {
                mouse_left_state_.state = Drawing;
                mouse_left_state_.type = DrawEllipse;
                break;
            }
        case DrawPolygon: {
                break;
            }
        case DrawClosedCurve: {
                break;
            }
        case DrawTextMark: {
                mouse_left_state_.state = Drawing;
                mouse_left_state_.type = DrawTextMark;
                GraphicsTextMarkDialog dialog(this);
                if ((dialog.exec() == QDialog::Accepted) &&
                        (!dialog.getText().isEmpty())) {
                    GraphicsTextMarkItem *item = new GraphicsTextMarkItem(pixmap_item_);
                    item->setFlag(QGraphicsItem::ItemIgnoresParentOpacity);
                    item->setLabelItemText(dialog.getText());
                    item->grabMouse();
                    current_path_item_ = item;
                }
                break;
            }
        case DrawArrow: {
                mouse_left_state_.state = Drawing;
                mouse_left_state_.type = DrawArrow;
                break;
            }
        case DrawCurve: {
                break;
            }
        case RestPan: {
                pixmap_item_->setPos(0, 0);
                manual_pan_ = false;
                centerOn(pixmap_item_);
                RepositionAuxItems();
                break;
            }
        case RemoveAllDraw: {
                qDeleteAll(item_list_);
                item_list_.clear();
                break;
            }
    }
}

//-------------------------------------------------------
void DicomImageView::SetOperation(const CurrentState &operation) {
    if (!m_series_) {
        return;
    }
    switch (operation) {
        case None:
            mouse_left_state_.state = None;
            break;
        case AdjustWL:
            m_scene_->clearSelection();
            mouse_left_state_.state = AdjustWL;
            break;
        case Zoom:
            m_scene_->clearSelection();
            mouse_left_state_.state = Zoom;
            break;
        case Pan:
            m_scene_->clearSelection();
            mouse_left_state_.state = Pan;
            manual_pan_ = true;
            break;
        case Slicing:
            m_scene_->clearSelection();
            mouse_left_state_.state = Slicing;
            break;
        case Magnifier:
            m_scene_->clearSelection();
            mouse_left_state_.state = Magnifier;
            break;
        case ROIWindow:
            m_scene_->clearSelection();
            mouse_left_state_.state = ROIWindow;
            break;
        case Drawing:
            break;
    }
}

//-------------------------------------------------------
void DicomImageView::SetOperation(
    const DicomImageView::WindowWLWHOperation &operation) {
    if (!m_series_) {
        return;
    }
    switch (operation) {
        case DefaultWL: {
                m_series_->SetDefaultWindow();
                RefreshPixmap();
                break;
            }
        case FullDynamic: {
                m_series_->SetFullDynamic();
                RefreshPixmap();
                break;
            }
        case InverseWl: {
                m_series_->SetPolarity(m_series_->GetPolarity()
                                       == EPP_Normal ? EPP_Reverse : EPP_Normal);
                RefreshPixmap();
                break;
            }
    }
}

//-------------------------------------------------------
/**
 * @brief DicomImageView::RemoveCurrentDrawingItem
 * 删除当前选择的item
 */
void DicomImageView::RemoveCurrentDrawingItem() {
    if (current_path_item_) {
        current_path_item_->ungrabMouse();
        delete current_path_item_;
        current_path_item_ = nullptr;
    }
    QList<QGraphicsItem *> items = m_scene_->selectedItems();
    foreach (QGraphicsItem *item, items) {
        if (item->parentItem() != pixmap_item_) {
            item = item->parentItem();
        }
        if (item->parentItem() == pixmap_item_) {
            //scene->removeItem(item);
            item_list_.removeOne(static_cast<AbstractPathItem *>(item));
            delete item;
        }
    }
}

//-------------------------------------------------------
/**
 * @brief DicomImageView::SetShowAnnotations
 * @param yes
 */
void DicomImageView::SetShowAnnotations(bool yes) {
    foreach (GraphicsAnnoGroup *g, anno_grps_) {
        foreach (QGraphicsSimpleTextItem *t, g->items) {
            t->setVisible(yes);
        }
    }
    x_scalor_item_->setVisible(yes);
    y_scalor_item_->setVisible(yes);
}

//-------------------------------------------------------
/**
 * @brief DicomImageView::SetShowMeasurements
 * @param yes
 */
void DicomImageView::SetShowMeasurements(bool yes) {
    foreach (AbstractPathItem *i, item_list_) {
        i->setVisible(yes);
    }
}

//-------------------------------------------------------
/**
 * @brief DicomImageView::Reset
 */
void DicomImageView::Reset() {
    pixmap_item_->setPos(0, 0);
    pixmap_item_->setRotation(0);
    rotate_angle_ = 0;
    pixmap_item_->resetTransform();
    hflip_ = false;
    vflip_ = false;
    manual_zoom_ = false;
    manual_pan_ = false;
    mouse_left_state_.state = None;
    if (m_series_) {
        if (m_series_->GetPolarity() != EPP_Normal) {
            m_series_->SetPolarity(EPP_Normal);
        }
        m_series_->SetDefaultWindow();
    }
    RefreshPixmap();
    this->SetOperation(FillViewport);
}

//-------------------------------------------------------
/**
 * @brief DicomImageView::Slot_SeriesDelate
 */
void DicomImageView::Slot_SeriesDelate() {
    SetSeriesInstance(nullptr);
}
