#include "importdcmfilethread.h"

#include <dao/KissDb>
#include <engine/KissEngine>
#include <global/KissGlobal>

#include "dcmtk/dcmdata/dcuid.h"

//----------------------------------------------------------------
ImportDcmFileThread::ImportDcmFileThread(ImportStudyModel * model, QObject * parent)
  : QThread(parent)
{
    this->abort_ = false;
    this->import_model_ = model;
}

//----------------------------------------------------------------
void ImportDcmFileThread::run()
{
    StudyDao dao;
    foreach (StudyRecord * study, import_model_->getStudyList()) {
        if (abort_) {
            break;
        }
        int images = 0;
        QString study_dir_name =
          QString("%1/%2_%3").arg(study->study_time_.date().toString("yyyyMM"), study->study_time_.toString(DICOM_DATETIME_FORMAT), study->acc_number_);
        if (!dao.VerifyStudyByStuid(study->study_uid_)) {
            dao.InsertStudyToDb(*study, true);
        }
        FileUtil::DirMake(QString("%1/%2").arg(DICOM_SAVE_PATH, study_dir_name));
        foreach (ImageRecord * image, study->image_list_) {
            bool raw = image->sop_class_uid_ == QString(UID_XRayAngiographicImageStorage);
            QString src_file = image->image_file_;
            image->image_file_ =
              QString("%1/%2%3.dcm")
                .arg(study_dir_name, raw ? "XA_" : "", Kiss::GetRandString());
            QFileInfo info(QString("%1/%2").arg(DICOM_SAVE_PATH, image->image_file_));
            if (FileUtil::FileCopy(src_file, QString("%1/%2").arg(DICOM_SAVE_PATH, image->image_file_))) {
                if (!dao.VerifyImageByIMmuid(image->image_uid_)) {
                    if (dao.InsertImageToDb(*image, true)) {
                        images++;
                    } else {
                    }
                } else {
                    if (dao.UpdateImageFile(image->image_uid_, image->image_file_)) {
                        images++;
                    } else {
                        FileUtil::DeleteFileOrFolder(
                          QString("%1/%2").arg(DICOM_SAVE_PATH, image->image_file_));
                    }
                }
            }
            image->image_file_ = src_file;
            emit SgnResultReady();
        }
        study->status_ = tr("Imported: Images %1.").arg(images);
        import_model_->resetStudyStatus(study);
    }
}

//----------------------------------------------------------------
void ImportDcmFileThread::SetAbort(bool yes)
{
    abort_ = yes;
}
