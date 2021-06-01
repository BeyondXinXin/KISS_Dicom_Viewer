#ifndef GRIDPOPWIDGET_H
#define GRIDPOPWIDGET_H

#include <QWidget>

namespace Ui {
class GridPopWidget;
}

class GridPopWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GridPopWidget(QWidget * parent = nullptr);
    ~GridPopWidget();

signals:
    void Signal_ViewLayout(int col, int row);

protected:
    void mousePressEvent(QMouseEvent * e);
    void mouseMoveEvent(QMouseEvent * e);
    void hideEvent(QHideEvent * e);

private:
    Ui::GridPopWidget * ui;

    QList<QList<QWidget *>> wArr;
};

#endif // GRIDPOPWIDGET_H
