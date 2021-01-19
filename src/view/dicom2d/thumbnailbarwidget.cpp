#include "thumbnailbarwidget.h"
#include "dicomimagelabel.h"

#include <script/KissScript>
#include <global/KissGlobal>
#include <engine/KissEngine>

#include "ImageData/seriesinstance.h"
#include "ImageData/imageinstance.h"
#include "filewatcherthread.h"
#include "dcmtk/dcmdata/dcuid.h"
#include <QTimer>

//-------------------------------------------------------
ThumbnailBarWidget::ThumbnailBarWidget(QWidget *parent) :
    QWidget(parent),
    currentImageLabel(nullptr) {
    setFocusPolicy(Qt::StrongFocus);
    layout = new QBoxLayout(QBoxLayout::TopToBottom, this);
    layout->setContentsMargins(6, 6, 6, 6);
    layout->addStretch();
    layout->setAlignment(Qt::AlignCenter);
}

//-------------------------------------------------------
ThumbnailBarWidget::~ThumbnailBarWidget() {
    clear();
    emit Signal_QuitFileWatcher();
}

//-------------------------------------------------------
void ThumbnailBarWidget::setCurrentImageLabel(const SeriesInstance *series) {
    if (currentImageLabel && currentImageLabel->getSeriesInstance() == series) {
        currentImageLabel->setHighlight(true);
    } else {
        bool found = false;
        foreach (DicomImageLabel *label, imageLabelList) {
            if (series == label->getSeriesInstance()) {
                if (currentImageLabel) {
                    currentImageLabel->setHighlight(false);
                }
                currentImageLabel = label;
                currentImageLabel->setHighlight(true);
                found = true;
                break;
            }
        }
        if ((!found) && currentImageLabel) {
            currentImageLabel->setHighlight(false);
            currentImageLabel = nullptr;
        }
    }
}

//-------------------------------------------------------
void ThumbnailBarWidget::firstSeries() {
    if (!imageLabelList.isEmpty()) {
        Slot_ImageDoubleClicked(imageLabelList.first());
    }
}

//-------------------------------------------------------
void ThumbnailBarWidget::lastSeries() {
    if (!imageLabelList.isEmpty()) {
        if ((!currentImageLabel) ||
                (currentImageLabel && currentImageLabel != imageLabelList.last())) {
            Slot_ImageDoubleClicked(imageLabelList.last());
        }
    }
}

//-------------------------------------------------------
void ThumbnailBarWidget::prevSeries() {
    if (currentImageLabel && (imageLabelList.size())) {
        int i = imageLabelList.indexOf(currentImageLabel);
        if (i > 0) {
            Slot_ImageDoubleClicked(imageLabelList.at(i - 1));
        }
    }
}

//-------------------------------------------------------
void ThumbnailBarWidget::nextSeries() {
    if (currentImageLabel && (imageLabelList.size())) {
        int i = imageLabelList.indexOf(currentImageLabel);
        if (i < imageLabelList.size() - 1) {
            Slot_ImageDoubleClicked(imageLabelList.at(i + 1));
        }
    }
}

//-------------------------------------------------------
void ThumbnailBarWidget::currSeries() {
    if (currentImageLabel) {
        emit Signal_ImageDoubleClicked(currentImageLabel->getSeriesInstance());
    }
}

void ThumbnailBarWidget::UpdataLabeSize() {
    foreach (DicomImageLabel *label, imageLabelList) {
        label->setFixedSize(label->sizeHint());
    }
}

//-------------------------------------------------------
void ThumbnailBarWidget::setFileWatcher(const QString &dir) {
    emit Signal_QuitFileWatcher();
    FileWatcherThread *t = new FileWatcherThread(dir);
    connect(this, &ThumbnailBarWidget::Signal_QuitFileWatcher,
            t, &FileWatcherThread::quit);
    connect(t, &FileWatcherThread::finished,
            t, &FileWatcherThread::deleteLater);
    connect(t, &FileWatcherThread::Signal_FilesChanged,
            this, &ThumbnailBarWidget::Slot_FilesChanged);
    t->start();
}

//-------------------------------------------------------
void ThumbnailBarWidget::updateLabelImage(const SeriesInstance *series) {
    if (currentImageLabel && currentImageLabel->getSeriesInstance() == series) {
        currentImageLabel->updateThumbnailImage();
        return;
    }
    foreach (DicomImageLabel *label, imageLabelList) {
        if (label->getSeriesInstance() == series) {
            label->updateThumbnailImage();
            break;
        }
    }
}

//-------------------------------------------------------
void ThumbnailBarWidget::setImagePaths(const QStringList &paths) {
    appendImagePaths(paths, true);
}

//-------------------------------------------------------
void ThumbnailBarWidget::appendImagePaths(const QStringList &paths, bool clear_old) {
    QStringList path_list = paths;
    QStringList files;
    QStringList unloaded_files;
    while (!path_list.isEmpty()) {
        QString p = path_list.takeFirst();
        QDir dir(p);
        if (dir.exists()) {
            QStringList subs = dir.entryList(QDir::Files);
            foreach (const QString &s, subs) {
                files += p + QDir::separator() + s;
            }
            subs = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
            foreach (const QString &s, subs) {
                path_list += p + QDir::separator() + s;
            }
        } else {
            files += p;
        }
    }
    if (clear_old) {
        unloaded_files = files;
        clear();
    } else {
        foreach (const QString &file, files) {
            bool found = false;
            foreach (DicomImageLabel *label, imageLabelList) {
                if (label->HasImage(file)) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                unloaded_files << file;
            }
        }
    }
    if(unloaded_files.size() > 3000) {
        this->Signal_ImageLoadFilesSize(unloaded_files.size());
        return;
    }
    emit Signal_ImageLoadBegin();
    OFLog::configure(OFLogger::WARN_LOG_LEVEL);
    foreach (const QString &p, unloaded_files) {
        QApplication::processEvents();
        Slot_ImagePathReady(p);
    }
    OFLog::configure(OFLogger::INFO_LOG_LEVEL);
    emit Signal_ImageLoadFinished();
}

//-------------------------------------------------------
void ThumbnailBarWidget::Slot_ImageReady(ImageInstance *image) {
    bool inserted = false;
    foreach (DicomImageLabel *label, imageLabelList) {
        if (label->insertImage(image)) {
            inserted = true;
            emit Signal_SeriesAppend();
            break;
        }
    }
    if ((!inserted) && image) {
        DicomImageLabel *imageLabel =
            new DicomImageLabel(new SeriesInstance(image->GetSeriesUid()));
        if( imageLabel->insertImage(image)) {
            connect(imageLabel, &DicomImageLabel::Signal_ImageClicked,
                    this, &ThumbnailBarWidget::SLot_ImageClicked);
            connect(imageLabel, &DicomImageLabel::Signal_ImageDoubleClicked,
                    this, &ThumbnailBarWidget::Slot_ImageDoubleClicked);
            layout->insertWidget(imageLabelList.size(), imageLabel);
            imageLabelList.append(imageLabel);
            if (!currentImageLabel) {
                currentImageLabel = imageLabel;
            }
            emit Signal_SeriesInserted(imageLabel->getSeriesInstance());
        } else {
            delete imageLabel;
        }
    }
}

void ThumbnailBarWidget::Slot_ImagePathReady(const QString path) {
    ImageInstance *image = new ImageInstance(path);
    bool inserted = false;
    foreach (DicomImageLabel *label, imageLabelList) {
        if (label->insertImage(image)) {
            inserted = true;
            emit Signal_SeriesAppend();
            break;
        }
    }
    if ((!inserted) && image) {
        DicomImageLabel *imageLabel =
            new DicomImageLabel(new SeriesInstance(image->GetSeriesUid()));
        if(imageLabel->insertImage(image)) {
            connect(imageLabel, &DicomImageLabel::Signal_ImageClicked,
                    this, &ThumbnailBarWidget::SLot_ImageClicked);
            connect(imageLabel, &DicomImageLabel::Signal_ImageDoubleClicked,
                    this, &ThumbnailBarWidget::Slot_ImageDoubleClicked);
            layout->insertWidget(imageLabelList.size(), imageLabel);
            imageLabelList.append(imageLabel);
            if (!currentImageLabel) {
                currentImageLabel = imageLabel;
            }
            emit Signal_SeriesInserted(imageLabel->getSeriesInstance());
        } else {
            delete imageLabel;
        }
    }
    QApplication::processEvents();
}


//-------------------------------------------------------
void ThumbnailBarWidget::Slot_FilesChanged(const QStringList &removed,
        const QStringList &added) {
    foreach (const QString &f, removed) {
        foreach (DicomImageLabel *l, imageLabelList) {
            if (l->removeImage(f) && l->getSeriesInstance()->IsEmpty()) {
                layout->removeWidget(l);
                imageLabelList.removeOne(l);
                if (currentImageLabel == l) {
                    currentImageLabel = nullptr;
                }
                l->deleteLater();
                break;
            }
        }
    }
    if ((!currentImageLabel) && (!imageLabelList.isEmpty())) {
        SLot_ImageClicked(imageLabelList.first());
    }
    appendImagePaths(added);
}

//-------------------------------------------------------
void ThumbnailBarWidget::SLot_ImageClicked(DicomImageLabel *imageLabel) {
    if (currentImageLabel != imageLabel) {
        if (currentImageLabel) {
            currentImageLabel->select_ = false;
            currentImageLabel->setHighlight(false);
        }
        currentImageLabel = imageLabel;
        if (currentImageLabel) {
            currentImageLabel->select_ = true;
            currentImageLabel->setHighlight(true);
        }
    }
}

//-------------------------------------------------------
void ThumbnailBarWidget::Slot_ImageDoubleClicked(DicomImageLabel *imageLabel) {
    if (currentImageLabel) {
        currentImageLabel->select_ = false;
        currentImageLabel->setHighlight(false);
    }
    currentImageLabel = imageLabel;
    if (currentImageLabel) {
        currentImageLabel->select_ = true;
        currentImageLabel->setHighlight(true);
        emit Signal_ImageDoubleClicked(
            currentImageLabel->getSeriesInstance());
    }
}

//-------------------------------------------------------
QList<SeriesInstance *> ThumbnailBarWidget::getSeriesList() const {
    QList<SeriesInstance *> seriesList;
    foreach (DicomImageLabel *label, imageLabelList) {
        seriesList << label->getSeriesInstance();
    }
    return seriesList;
}

//-------------------------------------------------------
QSize ThumbnailBarWidget::sizeHint() const {
    QMargins margin = layout->contentsMargins();
    int width = DicomImageLabel::getImageLabelSize();
    int height = DicomImageLabel::getImageLabelSize() * imageLabelList.size();
    if (imageLabelList.size() > 1) {
        height += (imageLabelList.size() - 1) * layout->spacing();
    }
    switch (layout->direction()) {
        case QBoxLayout::TopToBottom:
        case QBoxLayout::BottomToTop:
            return QSize(width + margin.left() + margin.right(),
                         height + margin.top() + margin.bottom());
        case QBoxLayout::LeftToRight:
        case QBoxLayout::RightToLeft:
            return QSize(width + margin.top() + margin.bottom(),
                         height + margin.left() + margin.right());
            /*default:
                return QSize();*/
    }
}

//-------------------------------------------------------
void ThumbnailBarWidget::clear() {
    qDeleteAll(imageLabelList);
    imageLabelList.clear();
    currentImageLabel = nullptr;
}
