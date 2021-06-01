#include "dicomviewer.h"
#include "ui_dicomviewer.h"

#include "logdbwidget.h"
#include "widget/dicomtagswidget.h"
#include <dao/KissDb>
#include <engine/KissEngine>
#include <global/KissGlobal>

#include <view/KissView>

#include "engine/PluganInterface.h"

//-----------------------------------------------
DicomViewer::DicomViewer(DicomViewer::ViewerType type, QWidget * parent)
  : QWidget(parent)
  , ui(new Ui::DicomViewer)
  , m_type_(type)
{
    ui->setupUi(this);
    this->Initial();
}

//-----------------------------------------------
DicomViewer::~DicomViewer()
{
    delete ui;
}

//-----------------------------------------------
void DicomViewer::SetDicomFile(const QString & path)
{
    ui->thumbnailBar->setImagePaths(QStringList() << path);
}

//----------------------------------------------------------------
void DicomViewer::SetDicomFile(const QStringList & path)
{
    ui->thumbnailBar->setImagePaths(QStringList() << path);
}

//-----------------------------------------------
void DicomViewer::Initial()
{
    ui->thumbnailBar->installEventFilter(this);
    ui->viewContainer->installEventFilter(this);
    QWidget::setWindowTitle(Project_NAME);
    this->InitViewType();
    this->SetupToolBar();
    this->SetupConnection();
}

//-----------------------------------------------
void DicomViewer::SetupToolBar()
{
    this->SetupFileTool();
    this->SetupExportTool();
    this->SetupGridTool();
    this->SetupAnnoTool();
    this->SetupAdjustTool();
    this->SetupPanTool();
    this->SetupSlicingTool();
    this->SetupMarksTool();
    this->SetupZoomTool();
    this->SetupFlipTool();
    this->SetupFullTool();
    this->SetupPlugin();
}

//-----------------------------------------------
void DicomViewer::SetupFileTool()
{
    // Menu
    QMenu * m;
    m = new QMenu(this);
    m->addAction(tr("Open DICOM folder"), this, [&] {
        QSettings s;
        QString p = s.value(OPEN_DIR_PATH, ".").toString();
        p = QFileDialog::getExistingDirectory(this, tr("Open dicom directory"), p);
        if (!p.isEmpty()) {
            s.setValue(OPEN_DIR_PATH, p);
            ui->thumbnailBar->setImagePaths(QStringList() << p);
        }
    });
    m->addAction(tr("Open DICOM file"), this, [&] {
        QSettings s;
        QString p = s.value(OPEN_FILE_PATH).toString();
        QStringList fs =
          QFileDialog::getOpenFileNames(this, tr("Open dicom files"), p);
        if (!fs.isEmpty()) {
            s.setValue(OPEN_DIR_PATH, fs.first());
            ui->thumbnailBar->setImagePaths(fs);
        }
    });
    m->addAction(tr("Close all"), this, [&] {
        ui->thumbnailBar->clear();
    });
    m->addSeparator();
    m->addAction(tr("Quit"), this, &DicomViewer::close);
    ui->fileBtn->setMenu(m);
    // connect
    connect(ui->fileBtn, &QToolButton::clicked, this, [&] {
        QSettings s;
        QString p = s.value(OPEN_DIR_PATH, ".").toString();
        p = QFileDialog::getExistingDirectory(this, tr("Open dicom directory"), p);
        if (!p.isEmpty()) {
            s.setValue(OPEN_DIR_PATH, p);
            ui->thumbnailBar->setImagePaths(QStringList() << p);
        }
    });
}

//-----------------------------------------------
void DicomViewer::SetupExportTool()
{
    // Menu
    QMenu * m;
    m = new QMenu(this);
    m->addAction(tr("Export image"), this, [&] {
        ui->viewContainer->ExportImage();
    });
    m->addAction(tr("Export images"), this, [&] {
        ui->viewContainer->ExportImages();
    });
    m->addAction(tr("Copy to clipboard"), this, [&] {
        ui->viewContainer->copyToClipboard();
    });
    ui->exportBtn->setMenu(m);
    // connect
    connect(ui->exportBtn, &QToolButton::clicked, this, [&] {
        ui->viewContainer->ExportImages();
    });
}

//-----------------------------------------------
void DicomViewer::SetupGridTool()
{
    // Menu
    QMenu * m;
    QAction * a;
    m = new QMenu(this);
    a = m->addAction(tr("Preview bar"), this, [&](bool value) {
        ui->scrollArea->setVisible(value);
    });
    a->setCheckable(true);
    a->setChecked(true);
    ui->gridBtn->setMenu(m);
    // connect
    connect(ui->gridBtn, &QToolButton::clicked, this, [&] {
        // auto delete while hidden
        GridPopWidget * gpw = new GridPopWidget(ui->gridBtn);
        connect(gpw, &GridPopWidget::SgnViewLayout,
                ui->viewContainer, &ViewContainerWidget::Slot_SetViewLayout);
        gpw->move(this->geometry().topLeft() + ui->gridBtn->geometry().bottomLeft() + QPoint(25, 25));
        gpw->show();
    });
}

//-----------------------------------------------
void DicomViewer::SetupAnnoTool()
{
    // Menu
    QMenu * m;
    QAction * a;
    m = new QMenu(this);
    a = m->addAction(tr("Annotations"));
    a->setCheckable(true);
    a->setChecked(true);
    a = m->addAction(tr("Measurements"), this, [&](bool value) {
        ui->viewContainer->showMeasurements(value);
    });
    a->setCheckable(true);
    a->setChecked(true);
    m->addSeparator();
    m->addAction(tr("Increase font"), this, [&] {
        ui->viewContainer->incAnnoFont();
    });
    m->addAction(tr("Decrease font"), this, [&] {
        ui->viewContainer->decAnnoFont();
    });
    m->addAction(tr("Select font"), this, [&] {
        ui->viewContainer->setAnnoFont();
    });
    m->addSeparator();
    m->addAction(tr("DICOM tags"), this, [&] {
        DicomTagsWidget * wid =
          new DicomTagsWidget { this->ui->viewContainer->GetCurrentImageFile() };
        wid->show();
    });
    ui->annoBtn->setMenu(m);
    // connect
    a = ui->annoBtn->menu()->actions().first();
    connect(ui->annoBtn, &QToolButton::clicked, a, &QAction::toggle);
    connect(a, &QAction::toggled, this, [&](bool value) {
        ui->viewContainer->showAnnotations(value);
    });
}

//-----------------------------------------------
void DicomViewer::SetupAdjustTool()
{
    // Menu
    QMenu * m;
    m = new QMenu(this);
    m->addAction(tr("Default"), this, [&] {
        ui->viewContainer->SetOperation(DicomImageView::DefaultWL);
    });
    m->addAction(tr("Full dynamic"), this, [&] {
        ui->viewContainer->SetOperation(DicomImageView::FullDynamic);
    });
    m->addSeparator();
    m->addAction(QIcon(":/png/contrast.png"), tr("Negative"), this, [&] {
        ui->viewContainer->SetOperation(DicomImageView::InverseWl);
    });
    ui->adjustBtn->setMenu(m);
    // connect
    connect(ui->adjustBtn, &QToolButton::clicked, this, [&] {
        ui->viewContainer->SetOperation(DicomImageView::AdjustWL);
    });
}

//-----------------------------------------------
void DicomViewer::SetupPanTool()
{
    // Menu
    QMenu * m;
    m = new QMenu(this);
    m->addAction(QIcon(":/png/reset.png"), tr("Reset"), this, [&] {
        ui->viewContainer->SetOperation(DicomImageView::RestPan);
    });
    ui->panBtn->setMenu(m);
    // connect
    connect(ui->panBtn, &QToolButton::clicked, this, [&] {
        ui->viewContainer->SetOperation(DicomImageView::Pan);
    });
}

//-----------------------------------------------
void DicomViewer::SetupSlicingTool()
{
    connect(ui->slicingBtn, &QToolButton::clicked, this, [&] {
        ui->viewContainer->SetOperation(DicomImageView::Slicing);
    });
}

//-----------------------------------------------
void DicomViewer::SetupMarksTool()
{
    // Menu
    QMenu * m;
    m = new QMenu(this);
    m->addAction(QIcon(":/png/line.png"), tr("Length"), this, [&] {
        ui->marksBtn->setIcon(QIcon(":/png/line.png"));
        ui->viewContainer->SetOperation(DicomImageView::DrawLine);
    });
    m->addAction(QIcon(":/png/ellipse.png"), tr("Ellipse"), this, [&] {
        ui->marksBtn->setIcon(QIcon(":/png/ellipse.png"));
        ui->viewContainer->SetOperation(DicomImageView::DrawEllipse);
    });
    m->addAction(QIcon(":/png/rect.png"), tr("Rectangle"), this, [&] {
        ui->marksBtn->setIcon(QIcon(":/png/rect.png"));
        ui->viewContainer->SetOperation(DicomImageView::DrawRect);
    });
    m->addAction(QIcon(":/png/angle.png"), tr("Angle"), this, [&] {
        ui->marksBtn->setIcon(QIcon(":/png/angle.png"));
        ui->viewContainer->SetOperation(DicomImageView::DrawAngle);
    });
    m->addAction(QIcon(":/png/text.png"), tr("Text"), this, [&] {
        ui->marksBtn->setIcon(QIcon(":/png/text.png"));
        ui->viewContainer->SetOperation(DicomImageView::DrawTextMark);
    });
    m->addSeparator();
    m->addAction(QIcon(":/png/recycle.png"), tr("Remove all"), this, [&] {
        ui->viewContainer->SetOperation(DicomImageView::RemoveAllDraw);
    });
    ui->marksBtn->setMenu(m);
    // connect
    connect(ui->marksBtn, &QToolButton::clicked, this, [&] {
        ui->marksBtn->setIcon(QIcon(":/png/line.png"));
        ui->viewContainer->SetOperation(DicomImageView::DrawLine);
    });
}

//-----------------------------------------------
void DicomViewer::SetupZoomTool()
{
    // Menu
    QMenu * m;
    m = new QMenu(this);
    m->addAction(tr("Fill viewport"), this, [&] {
        ui->viewContainer->SetOperation(DicomImageView::FillViewport);
    });
    m->addSeparator();
    m->addAction(tr("100%"), this, [&] {
        ui->viewContainer->SetOperation(DicomImageView::Zoom100);
    });
    m->addAction(tr("200%"), this, [&] {
        ui->viewContainer->SetOperation(DicomImageView::Zoom200);
    });
    m->addAction(tr("400%"), this, [&] {
        ui->viewContainer->SetOperation(DicomImageView::Zoom400);
    });
    m->addSeparator();
    m->addAction(QIcon(":/png/zoomin.png"), tr("Zoom in"), this, [&] {
        ui->viewContainer->SetOperation(DicomImageView::ZoomIn);
    });
    m->addAction(QIcon(":/png/zoomout.png"), tr("Zoom out"), this, [&] {
        ui->viewContainer->SetOperation(DicomImageView::Zoomout);
    });
    m->addSeparator();
    m->addAction(QIcon(":/png/magnifier.png"), tr("Magnifier"), this, [&] {
        ui->viewContainer->SetOperation(DicomImageView::Magnifier);
    });
    ui->zoomBtn->setMenu(m);
    // connect
    connect(ui->zoomBtn, &QToolButton::clicked, this, [&] {
        ui->viewContainer->SetOperation(DicomImageView::Zoom);
    });
}

//-----------------------------------------------
void DicomViewer::SetupFlipTool()
{
    // Menu
    QMenu * m;
    m = new QMenu(this);
    m->addAction(QIcon(":/png/rrotate.png"), tr("Rotate CW"), this, [&] {
        ui->viewContainer->SetOperation(DicomImageView::RoateCW);
    });
    m->addAction(QIcon(":/png/lrotate.png"), tr("Rotate CCW"), this, [&] {
        ui->viewContainer->SetOperation(DicomImageView::RoateCCW);
    });
    m->addAction(QIcon(":/png/reset.png"), tr("Rotate Reset"), this, [&] {
        ui->viewContainer->SetOperation(DicomImageView::ClearRoate);
    });
    m->addSeparator();
    m->addAction(QIcon(":/png/hflip.png"), tr("Flip horizontal"), this, [&] {
        ui->viewContainer->SetOperation(DicomImageView::HFlip);
    });
    m->addAction(QIcon(":/png/vflip.png"), tr("Flip vertical"), this, [&] {
        ui->viewContainer->SetOperation(DicomImageView::VFlip);
    });
    m->addAction(QIcon(":/png/reset.png"), tr("Flip Reset"), this, [&] {
        ui->viewContainer->SetOperation(DicomImageView::ClearFlip);
    });
    ui->flipBtn->setMenu(m);
    // connect
    connect(ui->flipBtn, &QToolButton::clicked, this, [&] {
        ui->viewContainer->SetOperation(DicomImageView::HFlip);
    });
}

//-----------------------------------------------
void DicomViewer::SetupFullTool()
{
    connect(ui->fullScreenBtn, &QToolButton::clicked, this, [&](bool checked) {
        if (checked) {
            QRect r = geometry();
            QSettings().setValue("WindowGeometry", r);
            setWindowState(Qt::WindowFullScreen);
            ui->fullScreenBtn->setIcon(QIcon(":/png/full_screen_exit.png"));
        } else {
            QRect r = QSettings().value("WindowGeometry").toRect();
            setWindowState(Qt::WindowNoState);
            if (r.isValid()) {
                this->setGeometry(r);
            }
            ui->fullScreenBtn->setIcon(QIcon(":/png/full_screen.png"));
        }
    });
}

//-----------------------------------------------
void DicomViewer::SetupConnection()
{
    // thumbnailBar <==> viewContainer
    connect(ui->thumbnailBar, SIGNAL(SgnSeriesInserted(SeriesInstance *)),
            ui->viewContainer, SLOT(SLot_SeriesInserted(SeriesInstance *)));
    connect(ui->thumbnailBar, SIGNAL(SgnImageDoubleClicked(SeriesInstance *)),
            ui->viewContainer, SLOT(Slot_ImageDClicked(SeriesInstance *)));
    connect(ui->thumbnailBar, SIGNAL(SgnSeriesAppend()),
            ui->viewContainer, SLOT(SLot_SeriesAppend()));
    // thumbnailBar <==> DicomViewer
    connect(ui->thumbnailBar, &ThumbnailBarWidget::SgnImageLoadBegin,
            this, [&] {
                ui->tool_widget->setVisible(0);
                this->update();
            });
    connect(ui->thumbnailBar, &ThumbnailBarWidget::SgnImageLoadFinished,
            this, [&] {
                ui->tool_widget->setVisible(1);
                ui->viewContainer->ImageLoadFinished();
            });
}

//-----------------------------------------------
void DicomViewer::SetupPlugin()
{
    QDir pluginsDir("./plugins");
    const auto entryList = pluginsDir.entryList(QDir::Files);
    for (const QString & fileName : entryList) {
        QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
        QObject * plugin = loader.instance();
        if (plugin) {
            const auto i_pretreatment =
              qobject_cast<PretreatmentInterface *>(plugin);
            if (i_pretreatment) {
                QMenu * m = ui->flipBtn->menu();
                QAction * a;
                QActionGroup * filter_group = new QActionGroup(this);
                QMenu * filter = new QMenu("filter", this);
                const QStringList texts = i_pretreatment->GetPretreatments();
                foreach (const QString var, texts) {
                    if (var.isEmpty()) {
                        filter->addSeparator();
                    } else {
                        a = filter->addAction(var);
                        Pretreatmen fun = i_pretreatment->PretreatmentFun(var);
                        connect(a, &QAction::triggered, this, [=] {
                            ui->viewContainer->SetPretreatmen(fun);
                        });
                        a->setCheckable(true);
                        // a->setChecked(true);
                        filter_group->addAction(a);
                    }
                }
                m->addMenu(filter);
            }
        }
    }
}

//-----------------------------------------------
void DicomViewer::InitViewType()
{
    if (this->m_type_ == Embed) {
        ui->fullScreenBtn->setVisible(0);
        ui->gridBtn->setVisible(0);
        qint32 size = ui->fileBtn->menu()->actions().size() - 1;
        QAction * a = ui->fileBtn->menu()->actions().at(size);
        ui->fileBtn->menu()->removeAction(a);
    } else if (this->m_type_ == SingleInstance) {
        LogDao dao;
        dao.InsertMessageToDao("",
                               LogDao::ET_SysInfo,
                               tr("Kiss Dicom Viewer Open"));
        this->SetWidgetDirection(QBoxLayout::LeftToRight);
    }
}

//-----------------------------------------------
void DicomViewer::SetWidgetDirection(const QBoxLayout::Direction & lay)
{
    if (lay == QBoxLayout::LeftToRight) {
        DicomImageLabel::setImage_label_size(180);
        this->update();
        ui->horizontalLayout_2->setDirection(QBoxLayout::LeftToRight);
        ui->scrollArea->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
        ui->thumbnailBar->setDirection(QBoxLayout::TopToBottom);
        ui->scrollArea->setMinimumSize(DicomImageLabel::getImage_label_size() + 40, 0);
    } else if (lay == QBoxLayout::TopToBottom) {
        DicomImageLabel::setImage_label_size(120);
        this->update();
        ui->horizontalLayout_2->setDirection(QBoxLayout::TopToBottom);
        ui->scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        ui->thumbnailBar->setDirection(QBoxLayout::LeftToRight);
        ui->scrollArea->setMinimumSize(0, DicomImageLabel::getImage_label_size() + 40);
    }
    ui->thumbnailBar->UpdataLabeSize();
    this->update();
}

//-----------------------------------------------
bool DicomViewer::eventFilter(QObject * watched, QEvent * event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent * e = static_cast<QKeyEvent *>(event);
        switch (e->key()) {
        case Qt::Key_Home:
            ui->thumbnailBar->firstSeries();
            break;
        case Qt::Key_End:
            ui->thumbnailBar->lastSeries();
            break;
        case Qt::Key_Return:
            ui->thumbnailBar->currSeries();
            break;
        case Qt::Key_Right:
            ui->thumbnailBar->nextSeries();
            break;
        case Qt::Key_Left:
            ui->thumbnailBar->prevSeries();
            break;
        case Qt::Key_Up:
            ui->viewContainer->prevFrame();
            break;
        case Qt::Key_Down:
            ui->viewContainer->nextFrame();
            break;
        default:
            return QObject::eventFilter(watched, event);
        }
        return true;
    } else {
        return QObject::eventFilter(watched, event);
    }
}
