#ifndef DICOMVIEWER_H
#define DICOMVIEWER_H


#include <QWidget>
#include <QBoxLayout>



class ThumbnailBarWidget;
class ViewContainerWidget;
class QLocalServer;

namespace Ui {
    class DicomViewer;
}

class DicomViewer : public QWidget {
    Q_OBJECT
  public:
    enum ViewerType {
        SingleInstance, // 单实例风格
        Embed,// 嵌入其他软件
    };
  public:
    explicit DicomViewer(
        ViewerType type = SingleInstance, QWidget *parent = nullptr);
    ~DicomViewer();
    void OpenDicomFolder();
  Q_SIGNALS:
    void Signal_ImageLoadFilesSize(const qint32 &num);
    void Signal_ImageLoadStageFinished();
  private slots:
    void SLot_OpenDicomFolder();
    void Slot_OpenDicomFile();
    void Slot_SplitView();
    void Slot_FullScreen(bool checked);
  protected:
    bool eventFilter(QObject *watched, QEvent *event);
  private:
    void SetupToolBar();
    void SetupConnection();
    void UpViewType();
  private:
    Ui::DicomViewer *ui;
    ViewerType m_type_;
  public:
    void RestWidget();
    void SetWidgetDirection(const QBoxLayout::Direction &lay);
    QString GetCurrentImageFile();
    qint32 GetCurrentImageNum();
    void SetDicomFile(const QString &path);
    void SetDicomFile(const QStringList &path);

};

#endif // DICOMVIEWER_H
