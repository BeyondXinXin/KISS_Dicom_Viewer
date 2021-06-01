#include "studyexplorerwidget.h"
#include "ui_studyexplorerwidget.h"

#include "dicomviewer.h"

#include "script/storescpthread.h"
#include "widget/importdcmwidget.h"
#include "widget/scpsettingview.h"

#include <dao/KissDb>
#include <engine/KissEngine>
#include <global/KissGlobal>

//----------------------------------------------------------------
StudyExplorerWidget::StudyExplorerWidget(QWidget * parent)
  : QWidget(parent)
  , ui(new Ui::StudyExplorerWidget)
{
    ui->setupUi(this);
    Initial();
}

//----------------------------------------------------------------
StudyExplorerWidget::~StudyExplorerWidget()
{
    store_scp_->terminate();
    delete ui;
}

//----------------------------------------------------------------
void StudyExplorerWidget::Slot_Today()
{
    ui->fromCheckBox->setChecked(true);
    ui->fromDateTimeEdit->setDate(QDate::currentDate());
    ui->fromDateTimeEdit->setTime(QTime(0, 0));
    ui->toCheckBox->setChecked(true);
    ui->toDateTimeEdit->setDate(QDate::currentDate());
    ui->toDateTimeEdit->setTime(QTime(23, 59, 59, 999));
    SetStudyFilter();
}

//----------------------------------------------------------------
void StudyExplorerWidget::Slot_LatestWeek()
{
    ui->fromCheckBox->setChecked(true);
    ui->fromDateTimeEdit->setDate(QDate::currentDate().addDays(-6));
    ui->fromDateTimeEdit->setTime(QTime(0, 0));
    ui->toCheckBox->setChecked(true);
    ui->toDateTimeEdit->setDate(QDate::currentDate());
    ui->toDateTimeEdit->setTime(QTime(23, 59, 59, 999));
    SetStudyFilter();
}

//----------------------------------------------------------------
void StudyExplorerWidget::Slot_LatestMonth()
{
    ui->fromCheckBox->setChecked(true);
    ui->fromDateTimeEdit->setDate(QDate::currentDate().addDays(-30));
    ui->fromDateTimeEdit->setTime(QTime(0, 0));
    ui->toCheckBox->setChecked(true);
    ui->toDateTimeEdit->setDate(QDate::currentDate());
    ui->toDateTimeEdit->setTime(QTime(23, 59, 59, 999));
    SetStudyFilter();
}

//----------------------------------------------------------------
void StudyExplorerWidget::Slot_ClearOptional()
{
    ui->fromCheckBox->setChecked(false);
    ui->toCheckBox->setChecked(false);
    ui->patientIDEdit->clear();
    ui->patientNameEdit->clear();
    ui->accNumberEdit->clear();
}

//----------------------------------------------------------------
void StudyExplorerWidget::SetStudyFilter()
{
    QString filter, temp;
    if (ui->fromCheckBox->isChecked()) {
        filter = QString("StudyTime>\'%1\'").arg(ui->fromDateTimeEdit->dateTime().toString("yyyy-MM-dd hh:mm:ss"));
    }
    if (ui->toCheckBox->isChecked()) {
        if (!filter.isEmpty()) {
            filter.append(" and ");
        }
        filter.append(QString("StudyTime<\'%1\'").arg(ui->toDateTimeEdit->dateTime().toString("yyyy-MM-dd hh:mm:ss")));
    }
    if (!ui->modalityCombo->currentText().isEmpty()) {
        if (!filter.isEmpty()) {
            filter.append(" and ");
        }
        filter.append(QString("Modality=\'%1\'").arg(ui->modalityCombo->currentText()));
    }
    if (!ui->patientIDEdit->text().isEmpty()) {
        temp = ui->patientIDEdit->text();
        temp.replace(QChar('*'), QChar('%'));
        temp.replace(QChar('?'), QChar('_'));
        if (!filter.isEmpty()) {
            filter.append(" and ");
        }
        filter.append(QString("PatientId LIKE \'%%1%\'").arg(temp));
    }
    if (!ui->patientNameEdit->text().isEmpty()) {
        temp = ui->patientNameEdit->text();
        temp.replace(QChar('*'), QChar('%'));
        temp.replace(QChar('?'), QChar('_'));
        if (!filter.isEmpty()) {
            filter.append(" and ");
        }
        filter.append(QString("PatientName LIKE \'%%1%\'").arg(temp));
    }
    if (!ui->accNumberEdit->text().isEmpty()) {
        temp = ui->accNumberEdit->text();
        temp.replace(QChar('*'), QChar('%'));
        temp.replace(QChar('?'), QChar('_'));
        if (!filter.isEmpty()) {
            filter.append(" and ");
        }
        filter.append(QString("AccNumber LIKE \'%%1%\'").arg(temp));
    }
    this->RefreshReadStudyModel(filter);
}

//----------------------------------------------------------------
void StudyExplorerWidget::RefreshReadStudyModel(const QString & filter)
{
    bool close = false;
    if (DbManager::IsOpenedDb()) {
    } else {
        if (DbManager::OpenDb()) {
            close = true;
        }
    }
    study_model_->setFilter(filter);
    study_model_->select();
    if (close) {
        DbManager::CloseDb();
    }
}

//----------------------------------------------------------------
void StudyExplorerWidget::Slot_StudySearch()
{
    this->SetStudyFilter();
}

//----------------------------------------------------------------
void StudyExplorerWidget::Initial()
{
    study_model_ = new SqlStudyModel(this, QSqlDatabase::database(DB_CONNECTION_NAME));
    study_model_->setTable(StudyDao::study_table_name_);
    study_view_ = new SqlStudyTabView(study_model_);
    study_model_->setFilter("StudyUid IS NULL");
    study_model_->select();
    study_view_->setModel(study_model_);
    ui->verticalLayout->insertWidget(3, study_view_, 1);
    //
    image_model_ = new SqlImageModel(this, QSqlDatabase::database(DB_CONNECTION_NAME));
    image_model_->setTable(StudyDao::image_table_name_);
    image_view_ = new SqlImageTabView(image_model_);
    image_model_->setFilter("ImageUid IS NULL");
    image_model_->select();
    image_view_->setModel(image_model_);
    ui->verticalLayout->insertWidget(4, image_view_);
    //
    store_scp_ = new StoreScpThread(this);
    store_scp_->start();
    //
    this->CreateConnections();
}

//----------------------------------------------------------------
void StudyExplorerWidget::CreateConnections()
{
    // UI
    connect(ui->btn_import, &QPushButton::clicked,
            this, &StudyExplorerWidget::Slot_LocalImportImage);
    connect(ui->btn_scpsetting, &QPushButton::clicked,
            this, &StudyExplorerWidget::Slot_ScpSetting);
    connect(ui->todayButton, &QPushButton::clicked,
            this, &StudyExplorerWidget::Slot_Today);
    connect(ui->latestWeekButton, &QPushButton::clicked,
            this, &StudyExplorerWidget::Slot_LatestWeek);
    connect(ui->latestMonthButton, &QPushButton::clicked,
            this, &StudyExplorerWidget::Slot_LatestMonth);
    connect(ui->clearButton, &QPushButton::clicked,
            this, &StudyExplorerWidget::Slot_ClearOptional);
    connect(ui->searchButton, &QPushButton::clicked,
            this, &StudyExplorerWidget::Slot_StudySearch);
    // view Images
    connect(study_view_, &SqlStudyTabView::Signal_ViewImages,
            image_model_, &SqlImageModel::SLot_ViewAllImages);
    connect(image_view_, &SqlImageTabView::Signal_ViewImages,
            image_model_, &SqlImageModel::SLot_ViewImages);
    connect(image_view_, &SqlImageTabView::Signal_ShowDirectories,
            image_model_, &SqlImageModel::SLot_ShowDirectories);
    connect(image_model_, &SqlImageModel::viewImages,
            this, [](QStringList file) {
                QStringList path;
                foreach (auto var, file) {
                    var = "./DcmFile/" + var;
                    path << var;
                }
                DicomViewer * w = new DicomViewer(DicomViewer::SingleInstance);
                w->SetDicomFile(path);
                w->show();
            });
    // remove Studies
    connect(study_view_, &SqlStudyTabView::Signal_RemoveStudies,
            study_model_, &SqlStudyModel::Slot_RemoveStudies);
    connect(study_model_, &SqlStudyModel::Signal_RemoveFinished,
            this, &StudyExplorerWidget::Slot_StudySearch);
    // remove Images
    connect(image_view_, &SqlImageTabView::Signal_RemoveImages,
            image_model_, &SqlImageModel::Slot_RemoveImages);
    connect(image_model_, &SqlImageModel::Signal_RemoveFinished,
            this, &StudyExplorerWidget::Slot_StudySearch);
    // Selection Changed
    connect(study_view_, &SqlStudyTabView::Singal_StudySelectionChanged,
            study_model_, &SqlStudyModel::Slot_SelectionChanged);
    connect(study_model_, &SqlStudyModel::Signal_studySelectionChanged,
            image_model_, &SqlImageModel::Slot_StudySelected);
    // PACS
    connect(store_scp_, &StoreScpThread::finished, this, [=] {
        QMessageBox::critical(this, QString("StoreScp 意外退出"), "软件将自动关闭");
        qApp->exit();
    });
}

//----------------------------------------------------------------
QStringList StudyExplorerWidget::getAllImageFiles() const
{
    return image_model_->getAllImageFiles();
}

//----------------------------------------------------------------
void StudyExplorerWidget::showEvent(QShowEvent * e)
{
    study_model_->select();
    QWidget::showEvent(e);
}

//----------------------------------------------------------------
void StudyExplorerWidget::Slot_LocalImportImage()
{
    ImportDcmWidget * w = new ImportDcmWidget();
    w->show();
}

//----------------------------------------------------------------
void StudyExplorerWidget::Slot_ScpSetting()
{
    ScpSettingView * w = new ScpSettingView();
    w->show();
}
