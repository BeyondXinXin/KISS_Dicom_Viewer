#ifndef KISSTABVIEW_H
#define KISSTABVIEW_H


#include <QTableView>

class KissTabView : public QTableView {
    Q_OBJECT
  public:
    explicit KissTabView(QAbstractTableModel *model, QWidget *parent = nullptr);
    explicit KissTabView(QWidget *parent = nullptr);
    ~KissTabView() {}
  public Q_SLOTS:
    QModelIndexList getSelectedIndexes() const;
  protected:
    void Initial();
  protected:
    void contextMenuEvent(QContextMenuEvent *e);
    QMenu *context_menu_;
    QItemSelection selection_;
};

#endif // KISSTABVIEW_H
