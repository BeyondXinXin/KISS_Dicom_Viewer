#ifndef GRAPHICSTEXTMARKDIALOG_H
#define GRAPHICSTEXTMARKDIALOG_H

#include <QDialog>

namespace Ui {
    class GraphicsTextMarkDialog;
}

class GraphicsTextMarkDialog : public QDialog {
    Q_OBJECT

  public:
    explicit GraphicsTextMarkDialog(QWidget *parent = nullptr);
    ~GraphicsTextMarkDialog();

    QString getText();

  private:
    Ui::GraphicsTextMarkDialog *ui;
};

#endif // GRAPHICSTEXTMARKDIALOG_H
