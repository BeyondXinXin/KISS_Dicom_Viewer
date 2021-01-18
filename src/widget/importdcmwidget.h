#ifndef IMPORTDCMWIDGET_H
#define IMPORTDCMWIDGET_H

#include <QWidget>

class ImportStudyModel;
class ImportStudyTabView;
class ImportDcmFileThread;
class ScanDcmFileThread;

namespace Ui {
    class ImportDcmWidget;
}

class ImportDcmWidget : public QWidget {
    Q_OBJECT
  public:
    explicit ImportDcmWidget(QWidget *parent = nullptr);
    ~ImportDcmWidget();
  protected:
    void closeEvent(QCloseEvent *e);
  private:
    void Initial();
    void CreateConnections();
    void SetImportButtonsDisabled(bool yes);

    void ScanFiles(const QStringList &files);
    void ScanDirHelpper(QStringList &files, const QString &dir);

  private:
    Ui::ImportDcmWidget *ui;

    ScanDcmFileThread *scan_dcmfile_thread_;
    ImportDcmFileThread *import_dcmfile_thread_;
    ImportStudyModel *import_study_model_;
    ImportStudyTabView *import_study_view_;
};

#endif // IMPORTDCMWIDGET_H
