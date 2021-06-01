#include "sqlstudytabview.h"

#include "../Model/sqlstudymodel.h"

#include <engine/KissEngine>
#include <global/KissGlobal>

//----------------------------------------------------------------
SqlStudyTabView::SqlStudyTabView(QAbstractTableModel * model, QWidget * parent)
  : KissTabView(model, parent)
{
    connect(this, &SqlStudyTabView::doubleClicked,
            this, &SqlStudyTabView::SgnViewImages);
    this->SetupContextMenu();
    this->HideColumns();
}

//----------------------------------------------------------------
void SqlStudyTabView::SetupContextMenu()
{
    context_menu_ = new QMenu(this);
    view_image_ = context_menu_->addAction(
      tr("View Image"), this, [=]() {
          if (!selectedIndexes().isEmpty()) {
              emit SgnViewImages();
          }
      });
    remove_study_ = context_menu_->addAction(
      tr("Remove Study"), this, [=]() {
          if (!selectedIndexes().isEmpty()) {
              if (QMessageBox::Ok == QMessageBox::question(this, tr("Remove Study"), tr("Are you sure to remove these studies?"), QMessageBox::Ok | QMessageBox::Cancel)) {
                  emit SgnRemoveStudies();
              }
          }
      });
}

//----------------------------------------------------------------
void SqlStudyTabView::HideColumns()
{
    this->hideColumn(SqlStudyModel::StudyUid);
}

//----------------------------------------------------------------
void SqlStudyTabView::selectionChanged(
  const QItemSelection & selected, const QItemSelection & deselected)
{
    QTableView::selectionChanged(selected, deselected);
    emit SgnStudySelectionChanged(selectedIndexes());
}

//----------------------------------------------------------------
void SqlStudyTabView::contextMenuEvent(QContextMenuEvent * e)
{
    QModelIndex index = indexAt(e->pos());
    if (index.isValid()) {
        context_menu_->popup(e->globalPos());
    }
}
