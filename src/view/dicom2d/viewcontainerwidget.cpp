#include "viewcontainerwidget.h"
#include "dicomimageview.h"
#include "exportimagedialog.h"

#include <global/KissGlobal>
#include <engine/KissEngine>

#include "ImageData/seriesinstance.h"
#include "ImageData/imageinstance.h"

#include "dcmtk/dcmdata/dcdeftag.h"


//-------------------------------------------------------
ViewContainerWidget::ViewContainerWidget(QWidget *parent) :
    QWidget(parent) {
    layout_ = new QGridLayout(this);
    current_view_ = nullptr;
    filter_ = nullptr;
    maxed_ = false;
    layout_->setContentsMargins(0, 0, 0, 0);
    layout_->setSpacing(0);
    QFont f;
    if (f.fromString(QSettings().value(ANNO_TEXT_FONT).toString())) {
        anno_font_ = f;
    }
    anno_font_.setPointSize(12);
    Slot_SetViewLayout(1, 1);
}

//-------------------------------------------------------
ViewContainerWidget::~ViewContainerWidget() {
    QSettings().setValue(ANNO_TEXT_FONT, anno_font_.toString());
}

//-------------------------------------------------------
QList<DicomImageView *> ViewContainerWidget::getViewList() const {
    return view_list_;
}

//-------------------------------------------------------
DicomImageView *ViewContainerWidget::getCurrentView() const {
    return current_view_;
}

//-------------------------------------------------------
void ViewContainerWidget::setEventFilter(QObject *filterObj) {
    installEventFilter(filterObj);
    filter_ = filterObj;
}

//-------------------------------------------------------
void ViewContainerWidget::SetOperation(
    const DicomImageView::ZoomOperation &operation) {
    if (current_view_) {
        current_view_->SetOperation(operation);
    }
}

//-------------------------------------------------------
void ViewContainerWidget::SetOperation(
    const DicomImageView::RoateFlipOperation &operation) {
    if (current_view_) {
        current_view_->SetOperation(operation);
    }
}

//-------------------------------------------------------
void ViewContainerWidget::SetOperation(
    const DicomImageView::DrawingType &operation) {
    if (current_view_) {
        current_view_->SetOperation(operation);
    }
}

//-------------------------------------------------------
void ViewContainerWidget::SetOperation(
    const DicomImageView::CurrentState &operation) {
    if (current_view_) {
        current_view_->SetOperation(operation);
    }
}

//-------------------------------------------------------
void ViewContainerWidget::SetOperation(
    const DicomImageView::WindowWLWHOperation &operation) {
    if (current_view_) {
        current_view_->SetOperation(operation);
    }
}

//-------------------------------------------------------
void ViewContainerWidget::SLot_SeriesAppend() {
    foreach (DicomImageView *v, view_list_) {
        if (v->HasSeries()) {
            v->SeriesInstanceAppend();
            break;
        }
    }
}

//-------------------------------------------------------
void ViewContainerWidget::SLot_SeriesInserted(SeriesInstance *series) {
    if (!series) {
        return;
    }
    DicomImageView *view = nullptr;
    foreach (DicomImageView *v, view_list_) {
        if (!v->HasSeries()) {
            v->SetSeriesInstance(series);
            view = v;
            break;
        }
    }
    if (current_view_ && current_view_ != view) {
        Slot_ViewDoubleClicked(view);
    }
}

//-------------------------------------------------------
void ViewContainerWidget::Slot_SeriesRemove(SeriesInstance *series) {
    foreach (DicomImageView *view, view_list_) {
        if (view->GetSeriesInstance() == series) {
            view->Slot_SeriesDelate();
        }
    }
}

//-------------------------------------------------------
void ViewContainerWidget::Slot_SeriesRemoveAll() {
    foreach (DicomImageView *view, view_list_) {
        view->Slot_SeriesDelate();
    }
}

//-------------------------------------------------------
void ViewContainerWidget::Slot_ImageDClicked(SeriesInstance *series) {
    if (series && series->GetTagKeyValue(DCM_Modality) == "MG") {
        SLot_SeriesInserted(series);
    } else {
        if (current_view_) {
            current_view_->SetSeriesInstance(series);
        }
    }
}

//-------------------------------------------------------
void ViewContainerWidget::Slot_ImageChanged(SeriesInstance *series) {
    if (!series) {
        return;
    }
    foreach (DicomImageView *v, view_list_) {
        if (v->GetSeriesInstance() == series) {
            SLot_ViewClicked(v);
            break;
        }
    }
}

//-------------------------------------------------------
void ViewContainerWidget::Slot_ViewImageChanged() {
    if (current_view_) {
        emit Signal_CurViewChanged(current_view_->GetSeriesInstance());
    }
}

//-------------------------------------------------------
/**
 * @brief ViewContainerWidget::onViewClicked
 * 单击view (切换 current view)
 * @param view
 */
void ViewContainerWidget::SLot_ViewClicked(DicomImageView *view) {
    if( current_view_ != view) {
        if (current_view_) {
            current_view_->SetBorderHighlight(false);
        }
        current_view_ = view;
        if (current_view_) {
            current_view_->SetBorderHighlight(true);
            emit Signal_CurViewChanged(current_view_->GetSeriesInstance());
        }
    }
}

//-------------------------------------------------------
/**
 * @brief ViewContainerWidget::Slot_ViewDoubleClicked
 * 双击 view(放大缩小)
 * @param view
 */
void ViewContainerWidget::Slot_ViewDoubleClicked(DicomImageView *view) {
    if (!view) {
        return;
    }
    if (maxed_) {
        foreach (DicomImageView *v, view_list_) {
            v->setVisible(true);
        }
        maxed_ = false;
    } else {
        foreach (DicomImageView *v, view_list_) {
            v->setVisible(false);
        }
        view->setVisible(true);
        maxed_ = true;
    }
}

//-------------------------------------------------------
/**
 * @brief ViewContainerWidget::Slot_SetViewLayout
 * 设置布局
 * @param col
 * @param row
 */
void ViewContainerWidget::Slot_SetViewLayout(int col, int row) {
    if (!(col > 0 && row > 0)) {
        return;
    }
    if (maxed_) {
        Slot_ViewDoubleClicked(current_view_);
    }
    int cellWidth = contentsRect().width() / col;
    int cellHeight = contentsRect().height() / row;
    foreach (DicomImageView *v, view_list_) {
        layout_->removeWidget(v);
    }
    int viewCount = col * row;
    while (viewCount < view_list_.size()) {
        DicomImageView *v = view_list_.takeLast();
        current_view_ = (current_view_ == v) ? nullptr : current_view_;
        delete v;
    }
    //
    DicomImageView *view;
    for (int i = 0; i < row; ++i) {
        for (int j = 0; j < col; ++j) {
            if (i * col + j < view_list_.size()) {
                view = view_list_[i * col + j];
            } else {
                view = createImageView();
                view_list_ << view;
            }
            layout_->addWidget(view, i, j);
        }
    }
    if ((!current_view_) && (!view_list_.isEmpty())) {
        SLot_ViewClicked(view_list_.first());
    }
    if (current_view_) {
        emit Signal_CurViewChanged(current_view_->GetSeriesInstance());
    }
}

//-------------------------------------------------------
DicomImageView *ViewContainerWidget::createImageView() {
    DicomImageView *v = new DicomImageView;
    v->SetAnnoTextFont(anno_font_);
    v->installEventFilter(filter_);
    connect(v, &DicomImageView::Signal_ViewClicked,
            this, &ViewContainerWidget::SLot_ViewClicked);
    connect(v, &DicomImageView::Singal_viewDoubleclicked,
            this, &ViewContainerWidget::Slot_ViewDoubleClicked);
    return v;
}

//-------------------------------------------------------
QString ViewContainerWidget::GetCurrentImageFile() {
    return this->current_view_->GetImageFile();
}

//-------------------------------------------------------
qint32 ViewContainerWidget::GetCurrentImageNum() {
    return this->current_view_->GetImageNum();
}

//-------------------------------------------------------
void ViewContainerWidget::clear() {
    foreach (DicomImageView *v, view_list_) {
        v->Slot_SeriesDelate();
        v->Reset();
        v->setVisible(true);
    }
    maxed_ = false;
}

//-------------------------------------------------------
void ViewContainerWidget::exportImages() {
    if (current_view_ && current_view_->GetSeriesInstance()) {
        ImageInstance *inst =
            current_view_->GetSeriesInstance()->GetCurrImageInstance(VT_XYPlane);
        if (inst) {
            ExportImageDialog d;
            d.SetImageFiles(QStringList() << inst->GetImageFile());
            d.exec();
        }
    }
}

//-------------------------------------------------------
void ViewContainerWidget::copyToClipboard() {
    if (current_view_) {
        QApplication::clipboard()->setPixmap(current_view_->getHardCopyPixmap());
    }
}

//-------------------------------------------------------
void ViewContainerWidget::showAnnotations(bool yes) {
    foreach (DicomImageView *v, view_list_) {
        v->SetShowAnnotations(yes);
    }
}

//-------------------------------------------------------
void ViewContainerWidget::showMeasurements(bool yes) {
    foreach (DicomImageView *v, view_list_) {
        v->SetShowMeasurements(yes);
    }
}

//-------------------------------------------------------
void ViewContainerWidget::incAnnoFont() {
    anno_font_.setPointSize(anno_font_.pointSize() + 1);
    foreach (DicomImageView *v, view_list_) {
        v->SetAnnoTextFont(anno_font_);
    }
}

//-------------------------------------------------------
void ViewContainerWidget::decAnnoFont() {
    anno_font_.setPointSize(anno_font_.pointSize() - 1);
    foreach (DicomImageView *v, view_list_) {
        v->SetAnnoTextFont(anno_font_);
    }
}

//-------------------------------------------------------
void ViewContainerWidget::setAnnoFont() {
    anno_font_ = QFontDialog::getFont(nullptr, anno_font_, this);
    foreach (DicomImageView *v, view_list_) {
        v->SetAnnoTextFont(anno_font_);
    }
}

//-------------------------------------------------------
void ViewContainerWidget::reset() {
    if (current_view_) {
        current_view_->Reset();
    }
}

//-------------------------------------------------------
void ViewContainerWidget::prevView() {
    if (current_view_ && view_list_.size()) {
        int i = view_list_.indexOf(current_view_);
        if (i > 0) {
            SLot_ViewClicked(view_list_.at(i - 1));
        }
    }
}

//-------------------------------------------------------
void ViewContainerWidget::nextView() {
    if (current_view_ && view_list_.size()) {
        int i = view_list_.indexOf(current_view_);
        if (i < view_list_.size() - 1) {
            SLot_ViewClicked(view_list_.at(i + 1));
        }
    }
}

//-------------------------------------------------------
void ViewContainerWidget::nextFrame() {
    if (current_view_) {
        current_view_->NextFrame();
    }
}

//-------------------------------------------------------
void ViewContainerWidget::prevFrame() {
    if (current_view_) {
        current_view_->PrevFrame();
    }
}

//-------------------------------------------------------
void ViewContainerWidget::resizeEvent(QResizeEvent *e) {
    QWidget::resizeEvent(e);
}
