#ifndef STUDYEXPLORERWIDGET_H
#define STUDYEXPLORERWIDGET_H

#include <QWidget>
class SqlStudyModel;
class SqlStudyTabView;
class SqlImageModel;
class SqlImageTabView;
class SqlReportModel;
class SqlReportView;
class QSortFilterProxyModel;
class DicomScp;
class StudyRecord;
class ImageRecord;
class ReportRecord;
class QSqlRecord;
class StoreScpThread;

namespace Ui {
    class StudyExplorerWidget;
}

class StudyExplorerWidget : public QWidget {
    Q_OBJECT

  public:
    explicit StudyExplorerWidget(QWidget *parent = nullptr);
    ~StudyExplorerWidget();
    QStringList getAllImageFiles() const;
  Q_SIGNALS:
    void viewImages(const QStringList &imageFiles);
    void studyModified(const QSqlRecord &study);
    void refreshStudyTable();
  public Q_SLOTS:
  protected:
    void showEvent(QShowEvent *e);
  private Q_SLOTS:
    void Slot_Today();
    void Slot_LatestWeek();
    void Slot_LatestMonth();
    void Slot_ClearOptional();
    void Slot_StudySearch();
    void Slot_LocalImportImage();
    void Slot_ScpSetting();

  private:
    void Initial();
    void CreateConnections();
    void SetStudyFilter();
    //
    void RefreshReadStudyModel(const QString &filter = "");

  private:
    Ui::StudyExplorerWidget *ui;

    SqlStudyModel *study_model_;
    SqlStudyTabView *study_view_;
    SqlImageModel *image_model_;
    SqlImageTabView *image_view_;

    StoreScpThread *store_scp_;

};

#endif // STUDYEXPLORERWIDGET_H
