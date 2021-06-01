#ifndef SQLIMAGETABVIEW_H
#define SQLIMAGETABVIEW_H

#include "kisstabview.h"

class SqlImageTabView : public KissTabView
{
    Q_OBJECT
public:
    explicit SqlImageTabView(QAbstractTableModel * model, QWidget * parent = nullptr);
    ~SqlImageTabView()
    {
    }
Q_SIGNALS:
    void SgnShowDirectories(const QModelIndexList & indexes);
    void SgnViewImages(const QModelIndexList & indexes);
    void SgnRemoveImages(const QModelIndexList & indexes);

private:
    void SetupContextMenu();
    void HideColumns();

private:
    QAction * view_image_action_;
    QAction * remove_image_action_;
    QAction * directories_;
};

#endif // SQLIMAGETABVIEW_H
