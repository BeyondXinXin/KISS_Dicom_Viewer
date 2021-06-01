#ifndef SCPSETTINGVIEW_H
#define SCPSETTINGVIEW_H

#include <QWidget>

namespace Ui {
class ScpSettingView;
}

class ScpSettingView : public QWidget
{
    Q_OBJECT

public:
    explicit ScpSettingView(QWidget * parent = nullptr);
    ~ScpSettingView();

private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

private:
    Ui::ScpSettingView * ui;
};

#endif // SCPSETTINGVIEW_H
