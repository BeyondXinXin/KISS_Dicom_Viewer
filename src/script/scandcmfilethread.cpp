#include "scandcmfilethread.h"
#include "global/global.h"
#include "global/studyrecord.h"

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcfilefo.h"
#include "dcmtk/dcmdata/dcuid.h"
#include "dcmtk/dcmimgle/dcmimage.h"
#include "dcmtk/dcmsr/dsrdoc.h"

//----------------------------------------------------------------
ScanDcmFileThread::ScanDcmFileThread(QObject * parent)
  : QThread(parent)
{
    this->abort_ = false;
}

//----------------------------------------------------------------
void ScanDcmFileThread::run()
{
    using namespace Kiss;
    foreach (QString file, file_list_) {
        if (abort_) {
            break;
        }
        StudyRecord * study = nullptr;
        DcmFileFormat dcmFile;
        OFCondition cond = dcmFile.loadFile(file.toLocal8Bit().data());
        DcmDataset * dset = dcmFile.getDataset();
        if (cond.good() && dset) {
            const char * value = nullptr;
            QString studyUid, seriesUid, instUid, sopClassUid;
            dset->findAndGetString(DCM_StudyInstanceUID, value);
            studyUid = QString::fromLatin1(value);
            dset->findAndGetString(DCM_SeriesInstanceUID, value);
            seriesUid = QString::fromLatin1(value);
            dset->findAndGetString(DCM_SOPInstanceUID, value);
            instUid = QString::fromLatin1(value);
            dset->findAndGetString(DCM_SOPClassUID, value);
            sopClassUid = QString::fromLatin1(value);
            if (!(studyUid.isEmpty() || seriesUid.isEmpty() || instUid.isEmpty() || sopClassUid.isEmpty())) {
                study = new StudyRecord(studyUid);
                dset->findAndGetString(DCM_AccessionNumber, value);
                study->acc_number_ = QString::fromLocal8Bit(value).remove(QChar(' '));
                dset->findAndGetString(DCM_PatientID, value);
                study->patient_id_ = QString::fromLocal8Bit(value);
                dset->findAndGetString(DCM_PatientName, value);
                study->patient_name_ = QString::fromLocal8Bit(value);
                dset->findAndGetString(DCM_PatientSex, value);
                study->patient_sex_ = QString::fromLocal8Bit(value).remove(QChar(' '));
                dset->findAndGetString(DCM_PatientBirthDate, value);
                study->patient_birth_ = QDate::fromString(QString::fromLatin1(value), "yyyyMMdd");
                dset->findAndGetString(DCM_PatientAge, value);
                study->patient_age_ = QString::fromLocal8Bit(value).remove(QChar(' '));
                dset->findAndGetString(DCM_StudyDate, value);
                study->study_time_.setDate(QDate::fromString(QString::fromLatin1(value), "yyyyMMdd"));
                dset->findAndGetString(DCM_StudyTime, value);
                study->study_time_.setTime(FormatDicomTime(QString::fromLatin1(value)));
                dset->findAndGetString(DCM_StudyDescription, value);
                study->study_desc_ = QString::fromLocal8Bit(value);
                dset->findAndGetString(DCM_InstitutionName, value);
                study->institution_ = QString::fromLocal8Bit(value);
                dset->findAndGetString(DCM_Modality, value);
                study->modality_ = QString::fromLatin1(value);
                if (sopClassUid == UID_XRayAngiographicImageStorage || // 造影血管
                    true) {
                    ImageRecord * image = new ImageRecord(instUid);
                    image->sop_class_uid_ = sopClassUid;
                    image->series_uid_ = seriesUid;
                    image->study_uid_ = studyUid;
                    image->image_file_ = file;
                    study->image_list_.append(image);
                    dset->findAndGetString(DCM_ReferencedSOPInstanceUID, value, true);
                    image->ref_image_uid_ = QString::fromLatin1(value);
                    dset->findAndGetString(DCM_InstanceNumber, value);
                    image->image_number_ = QString::fromLatin1(value);
                    dset->findAndGetString(DCM_SeriesDescription, value);
                    image->image_desc_ = QString::fromLocal8Bit(value);
                    dset->findAndGetString(DCM_ContentDate, value);
                    image->image_yime_.setDate(
                      QDate::fromString(QString::fromLatin1(value), "yyyyMMdd"));
                    dset->findAndGetString(DCM_ContentTime, value);
                    image->image_yime_.setTime(FormatDicomTime(QString::fromLatin1(value)));
                }
            }
        }
        if (study && (study->image_list_.isEmpty())) {
            delete study;
            study = nullptr;
        }
        emit SgnResultRecord(study);
        emit SgnResultReady();
    }
}

//----------------------------------------------------------------
void ScanDcmFileThread::SetFiles(const QStringList & files)
{
    file_list_ = files;
}

//----------------------------------------------------------------
void ScanDcmFileThread::SetAbort(bool yes)
{
    abort_ = yes;
}
