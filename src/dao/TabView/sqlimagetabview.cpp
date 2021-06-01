#include "sqlimagetabview.h"
#include "../Model/sqlimagemodel.h"
#include "global/stable.h"

//----------------------------------------------------------------
SqlImageTabView::SqlImageTabView(QAbstractTableModel * model, QWidget * parent)
  : KissTabView(model, parent)
{
    connect(this, &SqlImageTabView::doubleClicked,
            this, [=] {
                emit SgnViewImages(selectedIndexes());
            });
    this->SetupContextMenu();
    this->HideColumns();
}

//----------------------------------------------------------------
void SqlImageTabView::SetupContextMenu()
{
    view_image_action_ = context_menu_->addAction(
      tr("View Image"), this, [=]() {
          emit SgnViewImages(selectedIndexes());
      });
    remove_image_action_ = context_menu_->addAction(
      tr("Remove Image"), this, [=]() {
          if (QMessageBox::Ok == QMessageBox::question(this, tr("Remove Image"), tr("Are you sure to remove these images?"), QMessageBox::Ok | QMessageBox::Cancel)) {
              emit SgnRemoveImages(selectedIndexes());
          }
      });
    directories_ = context_menu_->addAction(
      tr("Show included directories"), this, [=]() {
          if (!selectedIndexes().isEmpty()) {
              emit SgnShowDirectories(selectedIndexes());
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
