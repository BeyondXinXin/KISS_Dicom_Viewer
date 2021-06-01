#ifndef DICOMIMAGELABEL_H
#define DICOMIMAGELABEL_H

#include "global/KissDicomViewConfig.h"
#include <QLabel>
#include <QMap>

class ImageInstance;
class SeriesInstance;

class DicomImageLabel : public QLabel
{
    Q_OBJECT
public:
    explicit DicomImageLabel(SeriesInstance * seriesPtr, QWidget * parent = nullptr);
    ~DicomImageLabel();

    QSize sizeHint() const
    {
        return QSize(image_label_size_, image_label_size_);
    }
    static int getImageLabelSize()
    {
        return image_label_size_;
    }
    SeriesInstance * getSeriesInstance() const
    {
        return m_series_;
    }
    bool HasImage(const QString & file) const;

    static qint32 getImage_label_size();
    static void setImage_label_size(const qint32 & value);
    bool select_ = false;
Q_SIGNALS:
    void SgnImageClicked(DicomImageLabel * label);
    void SgnImageDoubleClicked(DicomImageLabel * label);

public:
public Q_SLOTS:
    bool removeImage(const QString & imgFile);
    void setHighlight(bool yes);
    void updateThumbnailImage();
    bool insertImage(ImageInstance * image);

protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent * e);
    void mouseDoubleClickEvent(QMouseEvent * e);
    void mouseMoveEvent(QMouseEvent * e);

private:
    SeriesInstance * m_series_;
    QLabel * name_label_;
    QLabel * frame_label_;
    QPoint drag_org_;

    static qint32 image_label_size_;
};

#endif // DICOMIMAGELABEL_H
