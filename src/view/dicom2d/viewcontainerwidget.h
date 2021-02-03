#ifndef VIEWCONTAINERWIDGET_H
#define VIEWCONTAINERWIDGET_H

#include <QWidget>
#include "modalityproperty.h"
#include "dicomimageview.h"

class QGridLayout;
class DicomImageView;
class SeriesInstance;

class ViewContainerWidget : public QWidget {
    Q_OBJECT
  public:
    explicit ViewContainerWidget(QWidget *parent = nullptr);
    ~ViewContainerWidget();

    QList<DicomImageView *> getViewList() const;
    DicomImageView *getCurrentView() const;
    void setEventFilter(QObject *filterObj);
    void SetOperation(const DicomImageView::ZoomOperation &operation);// 缩放操作
    void SetOperation(const DicomImageView::RoateFlipOperation &operation);// 翻转操作
    void SetOperation(const DicomImageView::DrawingType &operation);// 标注操作
    void SetOperation(const DicomImageView::CurrentState &operation);// 鼠标状态操作
    void SetOperation(const DicomImageView::WindowWLWHOperation &operation);// WL 操作
    void SetPretreatmen(Pretreatmen fun);

    void incAnnoFont();
    void decAnnoFont();
    void setAnnoFont();
    void ImageLoadFinished();
  signals:
    void Signal_CurViewChanged(SeriesInstance *image);

  public slots:
    void SLot_SeriesAppend();
    void SLot_SeriesInserted(SeriesInstance *series);
    void Slot_SeriesRemove(SeriesInstance *series);
    void Slot_SeriesRemoveAll();
    void Slot_ImageDClicked(SeriesInstance *series);
    void Slot_ImageChanged(SeriesInstance *series);
    void Slot_ViewImageChanged();
    void SLot_ViewClicked(DicomImageView *view);
    void Slot_ViewDoubleClicked(DicomImageView *view);
    void Slot_SetViewLayout(int col, int row);
    void clear();
    // VIEW SLOTS
    void exportImages();
    void copyToClipboard();
    void showAnnotations(bool yes);
    void showMeasurements(bool yes);
    void reset();
    void prevView();
    void nextView();
    void nextFrame();
    void prevFrame();
  protected:
    void resizeEvent(QResizeEvent *e);

  private:
    DicomImageView *createImageView();
    void adjustViewArea();
  private:
    QList<DicomImageView *> view_list_;
    DicomImageView *current_view_;
    QFont anno_font_;
    QGridLayout *layout_;
    bool maxed_;// 当前widget是否最大化
    QObject *filter_;
  public:
    QString GetCurrentImageFile();
    qint32 GetCurrentImageNum();
};

#endif // VIEWCONTAINERWIDGET_H
