#ifndef DICOMVIEWER_H
#define DICOMVIEWER_H

#include <QBoxLayout>
#include <QWidget>

class ThumbnailBarWidget;
class ViewContainerWidget;
class QLocalServer;

namespace Ui {
class DicomViewer;
}

class DicomViewer : public QWidget
{
    Q_OBJECT
public:
    enum ViewerType
    {
        SingleInstance, // 单实例风格
        Embed, // 嵌入其他软件
    };

public:
    explicit DicomViewer(
      ViewerType type = SingleInstance, QWidget * parent = nullptr);
    ~DicomViewer();
    void SetDicomFile(const QString & path);
    void SetDicomFile(const QStringList & path);

protected:
    bool eventFilter(QObject * watched, QEvent * event);

private:
    void Initial();
    void SetupToolBar();
    void SetupFileTool();
    void SetupExportTool();
    void SetupGridTool();
    void SetupAnnoTool();
    void SetupAdjustTool();
    void SetupPanTool();
    void SetupSlicingTool();
    void SetupMarksTool();
    void SetupZoomTool();
    void SetupFlipTool();
    void SetupFullTool();
    void SetupConnection();
    void SetupPlugin();
    void InitViewType();
    void SetWidgetDirection(const QBoxLayout::Direction & lay);

private:
    Ui::DicomViewer * ui;
    ViewerType m_type_;
};

#endif // DICOMVIEWER_H
