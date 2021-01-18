#include "importdcmwidget.h"
#include "ui_importdcmwidget.h"

#include <dao/KissDb>
#include <global/KissGlobal>
#include <engine/KissEngine>

#include "script/importdcmfilethread.h"
#include "script/scandcmfilethread.h"

//-------------------------------------------------------
ImportDcmWidget::ImportDcmWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ImportDcmWidget) {
    ui->setupUi(this);
    this->Initial();
}

//-------------------------------------------------------
ImportDcmWidget::~ImportDcmWidget() {
    delete ui;
}

//-------------------------------------------------------
void ImportDcmWidget::closeEvent(QCloseEvent *e) {
    if (scan_dcmfile_thread_->isRunning() || import_dcmfile_thread_->isRunning()) {
        if (QMessageBox::Ok == QMessageBox::warning(this, tr("Abort Importing"),
                tr("Are you sure to abort importing?"),
                QMessageBox::Ok | QMessageBox::Cancel)) {
            scan_dcmfile_thread_->SetAbort(true);
            import_dcmfile_thread_->SetAbort(true);
            scan_dcmfile_thread_->wait(500);
            import_dcmfile_thread_->wait(500);
            e->accept();
        } else {
            e->ignore();
        }
    } else {
    }
}

//-------------------------------------------------------
void ImportDcmWidget::Initial() {
    import_study_model_ = new ImportStudyModel(this);
    import_study_view_ = new ImportStudyTabView(this);
    import_study_view_->setModel(import_study_model_);
    ui->importTabVLayout->insertWidget(0, import_study_view_);
    import_dcmfile_thread_ = new ImportDcmFileThread(import_study_model_, this);
    scan_dcmfile_thread_ = new ScanDcmFileThread(this);
    this->CreateConnections();
}

//-------------------------------------------------------
void ImportDcmWidget::CreateConnections() {
    // UI
    connect(ui->importButton, &QPushButton::clicked, this, [ = ](bool checked) {
        if (checked) { // Start import
            ui->progressBar->setValue(0);
            ui->progressBar->setMaximum(import_study_model_->getFileCount());
            import_dcmfile_thread_->SetAbort(false);
            import_dcmfile_thread_->start();
            ui->importButton->setChecked(true);
            ui->importButton->setText(tr("Abort"));
            SetImportButtonsDisabled(true);
        } else {
            if (scan_dcmfile_thread_->isRunning()) {  // Abort Scanning
                scan_dcmfile_thread_->SetAbort(true);
            } else  if (import_dcmfile_thread_->isRunning()) {  // Abort Import
                import_dcmfile_thread_->SetAbort(true);
            }
        }
    });
    connect(ui->fileButton, &QPushButton::clicked, this, [ = ] {
        QSettings s;
        QString file = s.value(STUDY_IMPORT_FILE).toString();
        if (file.isEmpty()) {
            file = ".";
        }
        QStringList files = QFileDialog::getOpenFileNames(this, tr("Import Study"),
                file, tr("DICOM Files(*.dcm);;All Files(*)"));
        if (files.size()) {
            s.setValue(STUDY_IMPORT_FILE, files.first());
            ScanFiles(files);
        }
    });
    connect(ui->folderButton, &QPushButton::clicked, this, [ = ] {
        QSettings s;
        QString dir = s.value(STUDY_IMPORT_FOLDER).toString();
        if (dir.isEmpty()) {
            dir = ".";
        }
        dir = QFileDialog::getExistingDirectory(this, tr("Import Study"), dir);
        if (!dir.isEmpty()) {
            s.setValue(STUDY_IMPORT_FOLDER, dir);
            QStringList files;
            ScanDirHelpper(files, dir);
            ScanFiles(files);
        }
    });
    connect(ui->removeButton,  &QPushButton::clicked,
            import_study_view_, &ImportStudyTabView::RemoveSelectedRows);
    connect(ui->clearStudyButton,  &QPushButton::clicked,
            import_study_view_,  &ImportStudyTabView::RemoveAllRows);
    // 数据库导入
    connect(import_dcmfile_thread_, &ImportDcmFileThread::finished, this, [ = ] {
        SetImportButtonsDisabled(false);
        ui->importButton->setChecked(false);
        ui->importButton->setText(tr("Import"));
    });
    connect(import_dcmfile_thread_, &ImportDcmFileThread::Signal_ResultReady, this, [ = ] {
        ui->progressBar->setValue(ui->progressBar->value() + 1);
    });
    // 文件检索
    connect(scan_dcmfile_thread_, &ScanDcmFileThread::finished, this, [ = ] {
        if (ui->importAfterScanCheck->isChecked()) {
            ui->progressBar->setValue(0);
            ui->progressBar->setMaximum(import_study_model_->getFileCount());
            import_dcmfile_thread_->SetAbort(false);
            import_dcmfile_thread_->start();
        } else {
            SetImportButtonsDisabled(false);
            ui->importButton->setChecked(false);
            ui->importButton->setText(tr("Import"));
        }
    });
    connect(scan_dcmfile_thread_, &ScanDcmFileThread::Signal_ResultReady, this, [ = ] {
        ui->progressBar->setValue(ui->progressBar->value() + 1);
    });
    connect(scan_dcmfile_thread_, SIGNAL(Signal_ResultRecord(StudyRecord *)),
            import_study_model_, SLOT(AppendStudy(StudyRecord *)));
}

//-------------------------------------------------------
void ImportDcmWidget::SetImportButtonsDisabled(bool yes) {
    ui->fileButton->setDisabled(yes);
    ui->folderButton->setDisabled(yes);
    ui->removeButton->setDisabled(yes);
    ui->clearStudyButton->setDisabled(yes);
}

//-------------------------------------------------------
void ImportDcmWidget::ScanFiles(const QStringList &files) {
    ui->progressBar->setMaximum(files.size());
    scan_dcmfile_thread_->SetAbort(false);
    scan_dcmfile_thread_->SetFiles(files);
    scan_dcmfile_thread_->start();
    SetImportButtonsDisabled(true);
    ui->importButton->setChecked(true);
    ui->importButton->setText(tr("Abort"));
}

//-------------------------------------------------------
void ImportDcmWidget::ScanDirHelpper(QStringList &files, const QString &dir) {
    QDir qdir(dir);
    QFileInfoList infoList = qdir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);
    foreach (QFileInfo info, infoList) {
        if (info.isFile()) {
            files.append(info.filePath());
        } else if (info.isDir()) {
            ScanDirHelpper(files, info.filePath());
        }
    }
}

