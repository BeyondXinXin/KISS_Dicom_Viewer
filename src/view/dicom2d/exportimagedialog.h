#ifndef EXPORTIMAGEDIALOG_H
#define EXPORTIMAGEDIALOG_H

#include <QDialog>
class ExportImageThread;

namespace Ui {
    class ExportImageDialog;
}

class ExportImageDialog : public QDialog {
    Q_OBJECT

  public:
    explicit ExportImageDialog(QWidget *parent = nullptr);
    ~ExportImageDialog();

    void SetImageFiles(const QStringList &images);

  protected:
    void closeEvent(QCloseEvent *e);

  protected slots:
    void Slot_Start(bool checked);
    void Slot_SelectFolder();
    void Slot_ExportFinished();
    void Slot_ResultReady(const QString &msg);

  private:
    void Initialization();
  private:
    Ui::ExportImageDialog *ui;
    ExportImageThread *export_thread_;
    int image_count_;
};

#endif // EXPORTIMAGEDIALOG_H
