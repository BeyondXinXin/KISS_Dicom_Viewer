#include "kisstabview.h"

#include <engine/KissEngine>
#include <global/KissGlobal>

//----------------------------------------------------------------
KissTabView::KissTabView(QAbstractTableModel *model, QWidget *parent):
    QTableView(parent)  {
    this->setModel(model);
    this->Initial();
}

//----------------------------------------------------------------
KissTabView::KissTabView(QWidget *parent) :
    QTableView(parent)  {
    this->Initial();
}

//----------------------------------------------------------------
QModelIndexList KissTabView::getSelectedIndexes() const {
    return this->selection_.indexes();
}

//----------------------------------------------------------------
void KissTabView::Initial() {
    horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    setSortingEnabled(true);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);
    context_menu_ = new QMenu(this);
}

//----------------------------------------------------------------
void KissTabView::contextMenuEvent(QContextMenuEvent *e) {
    if (indexAt(e->pos()).isValid()) {
        context_menu_->popup(e->globalPos());
    }
}


