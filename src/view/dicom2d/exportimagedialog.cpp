#include "exportimagedialog.h"
#include "ui_exportimagedialog.h"

#include <engine/KissEngine>
#include <global/KissGlobal>
#include <script/KissScript>

#define EXPORT_IMAGE_FOLDER "EXPORTIMAGEFOLDER"

//-------------------------------------------------------
ExportImageDialog::ExportImageDialog(QWidget * parent)
  : QDialog(parent)
  , ui(new Ui::ExportImageDialog)
  , export_thread_(new ExportImageThread(this))
  , image_count_(0)
{
    ui->setupUi(this);
    Initialization();
}

//-------------------------------------------------------
ExportImageDialog::~ExportImageDialog()
{
    delete ui;
}

//-------------------------------------------------------
void ExportImageDialog::closeEvent(QCloseEvent * e)
{
    if (export_thread_->isRunning()) {
        e->ignore();
    }
}

//-------------------------------------------------------
void ExportImageDialog::Slot_SelectFolder()
{
    QSettings settings;
    QString folder = settings.value(EXPORT_IMAGE_FOLDER, ".").toString();
    folder = QFileDialog::getExistingDirectory(this, tr("Select Destination Folder"), folder);
    if (!folder.isEmpty()) {
        settings.setValue(EXPORT_IMAGE_FOLDER, folder);
        ui->exportDestEdit->setText(folder);
    }
}

//-------------------------------------------------------
void ExportImageDialog::Slot_Start(bool checked)
{
    if (checked) {
        if (ui->exportDestEdit->text().isEmpty()) {
            Slot_SelectFolder();
            return;
        }
        if (ui->bmpRadio->isChecked()) {
            if (ui->bmp8BitRadio->isChecked()) {
                export_thread_->SetFileType(ExportImageThread::EFT_8bitBMP);
            } else if (ui->bmp24BitRadio->isChecked()) {
                export_thread_->SetFileType(ExportImageThread::EFT_24bitBMP);
            } else if (ui->bmp32BitRadio->isChecked()) {
                export_thread_->SetFileType(ExportImageThread::EFT_32bitBMP);
            } else {
                export_thread_->SetFileType(ExportImageThread::EFT_BMP);
            }
        } else if (ui->dicomRadio->isChecked()) {
            export_thread_->SetFileType(ExportImageThread::EFT_Dicom);
        } else if (ui->jpegRadio->isChecked()) {
            export_thread_->SetFileType(ExportImageThread::EFT_JPEG);
            export_thread_->SetJpegQuality(ui->jpegQualitySlider->value());
        } else if (ui->tiffRadio->isChecked()) {
            export_thread_->SetFileType(ExportImageThread::EFT_TIFF);
        } else if (ui->pnmRadio->isChecked()) {
            if (ui->pnm8BitRadio->isChecked()) {
                export_thread_->SetFileType(ExportImageThread::EFT_8bitPNM);
            } else if (ui->pnm16BitRadio->isChecked()) {
                export_thread_->SetFileType(ExportImageThread::EFT_16bitPNM);
            } else if (ui->pnmPastelRadio->isChecked()) {
                export_thread_->SetFileType(ExportImageThread::EFT_PastelPNM);
            } else {
                export_thread_->SetFileType(ExportImageThread::EFT_RawPNM);
            }
        } else if (ui->pngRadio->isChecked()) {
            export_thread_->SetFileType(ExportImageThread::EFT_PNG);
        } else {
            return;
        }
        ui->selectFolderButton->setDisabled(true);
        ui->closeBtn->setDisabled(true);
        ui->startBtn->setText(tr("Abort"));
        ui->progressBar->setMaximum(image_count_);
        ui->progressBar->setValue(0);
        ui->textBrowser->append(tr("Start exporting..."));
        export_thread_->SetAbort(false);
        export_thread_->SetDestDir(ui->exportDestEdit->text());
        export_thread_->start();
    } else {
        export_thread_->SetAbort(true);
    }
}

//-------------------------------------------------------
void ExportImageDialog::Slot_ExportFinished()
{
    ui->selectFolderButton->setDisabled(false);
    ui->startBtn->setText(tr("Start"));
    ui->startBtn->setChecked(false);
    ui->closeBtn->setDisabled(false);
}

//-------------------------------------------------------
void ExportImageDialog::Slot_ResultReady(const QString & msg)
{
    ui->progressBar->setValue(ui->progressBar->value() + 1);
    ui->textBrowser->append(msg);
}

//-------------------------------------------------------
void ExportImageDialog::SetMultiplePng(const QStringList & images)
{
    export_thread_->SetMultiplePng(images);
    image_count_ = images.size();
    ui->textBrowser->append(tr("%1 images to export.").arg(image_count_));
}

//-------------------------------------------------------
void ExportImageDialog::SetLeafletPng(const bool & leaflet,
                                      const int & frame, const QString & image)
{
    export_thread_->SetLeafletPng(leaflet, frame, image);
    image_count_ = 1;
    ui->textBrowser->append(tr("%1 images to export.").arg(image_count_));
}

//-------------------------------------------------------
void ExportImageDialog::Initialization()
{
    ui->bmpGroup->setVisible(false);
    ui->pnmGroup->setVisible(false);
    //
    connect(ui->bmpRadio, SIGNAL(toggled(bool)),
            ui->bmpGroup, SLOT(setVisible(bool)));
    connect(ui->pnmRadio, SIGNAL(toggled(bool)),
            ui->pnmGroup, SLOT(setVisible(bool)));
    connect(ui->jpegRadio, SIGNAL(toggled(bool)),
            ui->jpegGroup, SLOT(setVisible(bool)));
    //
    connect(ui->selectFolderButton, &QPushButton::clicked,
            this, &ExportImageDialog::Slot_SelectFolder);
    connect(ui->startBtn, &QPushButton::clicked,
            this, &ExportImageDialog::Slot_Start);
    connect(export_thread_, &ExportImageThread::finished,
            this, &ExportImageDialog::Slot_ExportFinished);
    connect(export_thread_, &ExportImageThread::resultReady,
            this, &ExportImageDialog::Slot_ResultReady);
    //
    ui->exportDestEdit->setText(QSettings().value(EXPORT_IMAGE_FOLDER).toString());
}
