#ifndef SQLSTUDYTABVIEW_H
#define SQLSTUDYTABVIEW_H

#include "kisstabview.h"


class SqlStudyTabView : public KissTabView {
    Q_OBJECT
  public:
    explicit SqlStudyTabView(QAbstractTableModel *model, QWidget *parent = nullptr);
    ~SqlStudyTabView() {}
  Q_SIGNALS:
    void Signal_ViewImages();
    void Signal_RemoveStudies();
    void Singal_StudySelectionChanged(const QModelIndexList &indexes);
  protected slots:
    void selectionChanged(const QItemSelection &selected,
                          const QItemSelection &deselected);
    void contextMenuEvent(QContextMenuEvent *e);
  private:
    void SetupContextMenu();
    void HideColumns();
  private:
    QStringList study_uids_;
    QAction *view_image_;
    QAction *remove_study_;
};


#endif // SQLSTUDYTABVIEW_H
