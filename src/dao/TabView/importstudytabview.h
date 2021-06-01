#ifndef IMPORTSTUDYTABVIEW_H
#define IMPORTSTUDYTABVIEW_H

#include "kisstabview.h"

class ImportStudyTabView : public KissTabView
{
    Q_OBJECT
public:
    explicit ImportStudyTabView(QWidget * parent = nullptr);
    ~ImportStudyTabView()
    {
    }
public Q_SLOTS:
    void RemoveSelectedRows();
    void RemoveAllRows();
protected Q_SLOTS:
    void selectionChanged(const QItemSelection & selected, const QItemSelection & deselected);
};

#endif // IMPORTSTUDYTABVIEW_H
