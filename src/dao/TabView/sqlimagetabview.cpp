#include "sqlimagetabview.h"

#include "../Model/sqlimagemodel.h"

#include <engine/KissEngine>
#include <global/KissGlobal>

//----------------------------------------------------------------
SqlImageTabView::SqlImageTabView(QAbstractTableModel * model, QWidget * parent)
  : KissTabView(model, parent)
{
    connect(this, &SqlImageTabView::doubleClicked,
            this, [=] {
                emit Signal_ViewImages(selectedIndexes());
            });
    this->SetupContextMenu();
    this->HideColumns();
}

//----------------------------------------------------------------
void SqlImageTabView::SetupContextMenu()
{
    view_image_action_ = context_menu_->addAction(
      tr("View Image"), this, [=]() {
          emit Signal_ViewImages(selectedIndexes());
      });
    remove_image_action_ = context_menu_->addAction(
      tr("Remove Image"), this, [=]() {
          if (QMessageBox::Ok == QMessageBox::question(this, tr("Remove Image"), tr("Are you sure to remove these images?"), QMessageBox::Ok | QMessageBox::Cancel)) {
              emit Signal_RemoveImages(selectedIndexes());
          }
      });
    directories_ = context_menu_->addAction(
      tr("Show included directories"), this, [=]() {
          if (!selectedIndexes().isEmpty()) {
              emit Signal_ShowDirectories(selectedIndexes());
          }
      });
}

//----------------------------------------------------------------
void SqlImageTabView::HideColumns()
{
    this->hideColumn(SqlImageModel::ImageUid);
    this->hideColumn(SqlImageModel::SopClassUid);
    this->hideColumn(SqlImageModel::SeriesUid);
    this->hideColumn(SqlImageModel::StudyUid);
    this->hideColumn(SqlImageModel::RefImageUid);
}
