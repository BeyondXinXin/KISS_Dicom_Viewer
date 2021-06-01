#ifndef THUMBNAILBARWIDGET_H
#define THUMBNAILBARWIDGET_H

#include <QBoxLayout>
#include <QPointer>
#include <QThread>
#include <QWidget>

class ImageInstance;
class SeriesInstance;
class DicomImageLabel;
class ImageLoadThread;
class DcmFileFormat;

class UnzipDicomFile : public QThread
{
    Q_OBJECT
public:
    explicit UnzipDicomFile()
    {
    }
    virtual ~UnzipDicomFile() override
    {
    }
    virtual void run() override;
    void SetPath(const QString & path);

private:
    QString path_;
};

class ThumbnailBarWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ThumbnailBarWidget(QWidget * parent = nullptr);
    ~ThumbnailBarWidget();

    void setDirection(QBoxLayout::Direction direction)
    {
        layout->setDirection(direction);
    }
    DicomImageLabel * getCurrentImageLabel() const
    {
        return currentImageLabel;
    }

    QList<SeriesInstance *> getSeriesList() const;
    int getNumberOfLabels() const
    {
        return imageLabelList.size();
    }
    QSize sizeHint() const;

Q_SIGNALS:
    void Signal_CurrentChanged(SeriesInstance * series);
    void Signal_ImageDoubleClicked(SeriesInstance * series);
    void Signal_SeriesInserted(SeriesInstance * series);
    void Signal_SeriesAppend();
    void Signal_QuitFileWatcher();
    void Signal_ImageLoadFinished();
    void Signal_ImageLoadBegin();

public Q_SLOTS:
    void setCurrentImageLabel(const SeriesInstance * series);
    void appendImagePaths(const QStringList & paths, bool clear_old = false);
    void setImagePaths(const QStringList & paths);
    void clear();
    void updateLabelImage(const SeriesInstance * series);
    void setFileWatcher(const QString & dir);
    void firstSeries();
    void lastSeries();
    void nextSeries();
    void prevSeries();
    void currSeries();
    void UpdataLabeSize();
protected Q_SLOTS:
    void Slot_ImageReady(ImageInstance * image);
    void Slot_ImagePathReady(const QString path);
    void SLot_ImageClicked(DicomImageLabel * imageLabel);
    void Slot_ImageDoubleClicked(DicomImageLabel * imageLabel);
    void Slot_FilesChanged(const QStringList & removed, const QStringList & added);

private:
    QBoxLayout * layout;
    QList<DicomImageLabel *> imageLabelList;
    DicomImageLabel * currentImageLabel;
    QThread workerThread;
};

#endif // THUMBNAILBARWIDGET_H
