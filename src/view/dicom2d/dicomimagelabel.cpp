#include "dicomimagelabel.h"
#include "ImageData/seriesinstance.h"

#include <global/KissGlobal>
#include <engine/KissEngine>

#include "dcmtk/dcmdata/dcdeftag.h"

qint32 DicomImageLabel::image_label_size_ = 120;

//-------------------------------------------------------
DicomImageLabel::DicomImageLabel(SeriesInstance *seriesPtr, QWidget *parent) :
    QLabel(parent) {
    m_series_ = seriesPtr;
    name_label_ = new QLabel(this);
    frame_label_ = new QLabel(this);
    setStyleSheet(QString::fromLatin1(""
                                      "background-color: rgb(0, 0, 0);"
                                      "font-size:12px;"
                                      "color:rgba(160,174,184,1);"));
    setFrameShape(QFrame::Box);
    setFrameShadow(QFrame::Plain);
    setLineWidth(2);
    setAlignment(Qt::AlignCenter);
    setScaledContents(true);
    name_label_->setFixedSize(110, 13);
}

//-------------------------------------------------------
DicomImageLabel::~DicomImageLabel() {
    delete m_series_;
}

//-------------------------------------------------------
bool DicomImageLabel::HasImage(const QString &file) const {
    return m_series_ && m_series_->HasImage(file);
}

//-------------------------------------------------------
bool DicomImageLabel::insertImage(ImageInstance *image) {
    if (m_series_ && m_series_->InsertImage(image)) {
        if (m_series_->GetFrameCount(VT_XYPlane) > 0) {
            QPixmap pixmap;
            if (m_series_->GetPixmap(pixmap, VT_XYPlane)) {
                setPixmap(pixmap.scaled(image_label_size_ - 2, image_label_size_ - 2,
                                        Qt::KeepAspectRatio, Qt::SmoothTransformation));
            }
            name_label_->setText(m_series_->GetTagKeyValue(DCM_PatientName));
#if HideNmae==1
            name_label_->setText("hide name");
#endif
            name_label_->move(4, 4);
        }
        QString tag = QString("%1: %2-%3")
                      .arg(m_series_->GetTagKeyValue(DCM_Modality),
                           m_series_->GetTagKeyValue(DCM_SeriesNumber),
                           QString::number(m_series_->GetFrameCount(VT_XYPlane)));
        frame_label_->setText(tag);
        QSize tagSize = frame_label_->sizeHint();
        frame_label_->resize(tagSize);
        frame_label_->move(image_label_size_ - tagSize.width(),
                           image_label_size_ - tagSize.height());
        return true;
    }
    return false;
}

//-------------------------------------------------------
bool DicomImageLabel::removeImage(const QString &imgFile) {
    return m_series_ && m_series_->RemoveImage(imgFile);
}

//-------------------------------------------------------
void DicomImageLabel::setHighlight(bool yes) {
    QPalette p = palette();
    if (yes) {
        p.setColor(QPalette::Window, Qt::green);
    } else {
        p.setColor(QPalette::Window, Qt::black);
    }
    setPalette(p);
}

//-------------------------------------------------------
void DicomImageLabel::updateThumbnailImage() {
    if (m_series_) {
        QPixmap pixmap;
        if (m_series_->GetPixmap(pixmap, VT_XYPlane)) {
            setPixmap(pixmap.scaled(image_label_size_ - 2,
                                    image_label_size_ - 2,
                                    Qt::KeepAspectRatio,
                                    Qt::SmoothTransformation));
        }
    }
}

void DicomImageLabel::paintEvent(QPaintEvent *e) {
    QLabel::paintEvent(e);
    if(select_) {
        QPainter painter(this);
        painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
        painter.save();
        painter.setPen(QPen(QColor(32, 218, 208), 4));
        painter.drawRect(rect());
        painter.restore();
    }
}

//-------------------------------------------------------
void DicomImageLabel::mousePressEvent(QMouseEvent *e) {
    emit Signal_ImageClicked(this);
    drag_org_ = e->pos();
    QLabel::mousePressEvent(e);
}

//-------------------------------------------------------
void DicomImageLabel::mouseMoveEvent(QMouseEvent *e) {
    if ((e->buttons() & Qt::LeftButton) &&
            ((e->pos() - drag_org_).manhattanLength() >
             QApplication::startDragDistance())) {
        QDrag *drag = new QDrag(this);
        QMimeData *data = new QMimeData;
        data->setText(QString::number((qulonglong)m_series_));
        drag->setMimeData(data);
        drag->exec(Qt::CopyAction);
    }
    QLabel::mouseMoveEvent(e);
}

qint32 DicomImageLabel::getImage_label_size() {
    return image_label_size_;
}

void DicomImageLabel::setImage_label_size(const qint32 &value) {
    image_label_size_ = value;
}

//-------------------------------------------------------
void DicomImageLabel::mouseDoubleClickEvent(QMouseEvent *e) {
    emit Signal_ImageDoubleClicked(this);
    QLabel::mouseDoubleClickEvent(e);
}
