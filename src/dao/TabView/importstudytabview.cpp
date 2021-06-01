#include "importstudytabview.h"

//----------------------------------------------------------------
ImportStudyTabView::ImportStudyTabView(QWidget * parent)
  : KissTabView(parent)
{
}

//----------------------------------------------------------------
void ImportStudyTabView::RemoveSelectedRows()
{
    QAbstractItemModel * model = this->model();
    if (model) {
        QList<int> rows;
        QModelIndexList indexList = selection_.indexes();
        foreach (QModelIndex index, indexList) {
            if (index.column() == 0) {
                rows << index.row();
            }
        }
        qSort(rows.begin(), rows.end());
        for (int i = rows.size() - 1; i >= 0; i--) {
            model->removeRow(rows.at(i));
        }
    }
}

//----------------------------------------------------------------
void ImportStudyTabView::RemoveAllRows()
{
    QAbstractItemModel * model = this->model();
    if (model) {
        model->removeRows(0, model->rowCount());
    }
}

//----------------------------------------------------------------
void ImportStudyTabView::selectionChanged(
  const QItemSelection & selected, const QItemSelection & deselected)
{
    selection_.merge(selected, QItemSelectionModel::Select);
    selection_.merge(deselected, QItemSelectionModel::Deselect);
    QTableView::selectionChanged(selected, deselected);
}
