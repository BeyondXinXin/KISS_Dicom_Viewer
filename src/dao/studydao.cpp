#include "studydao.h"

#include <global/KissGlobal>
#include <engine/KissEngine>

//----------------------------------------------------------------
const QString StudyDao::study_table_name_ = "StudyTable";
const QString StudyDao::image_table_name_ = "ImageTable";

//----------------------------------------------------------------
StudyDao::StudyDao(QObject *parent):
    QObject(parent) {
}

//----------------------------------------------------------------
StudyDao::~StudyDao() {
}

//----------------------------------------------------------------
bool StudyDao::InsertStudyToDb(const StudyRecord &study, bool imported) {
    Q_UNUSED(imported)
    bool success = false;
    if(DbManager::OpenDb()) {
        QMap<QString, QVariant> data;
        data.insert("StudyUid", study.study_uid_);
        data.insert("AccNumber", study.acc_number_);
        data.insert("PatientId", study.patient_id_);
        data.insert("PatientName", study.patient_name_);
        data.insert("PatientSex", study.patient_sex_);
        if(study.patient_birth_.toString("yyyy-MM-dd").isEmpty()) {
            data.insert("PatientBirth", "");
        } else {
            data.insert("PatientBirth", study.patient_birth_.toString("yyyy-MM-dd"));
        }
        data.insert("PatientAge", study.patient_age_);
        data.insert("StudyTime", study.study_time_.toString(NORMAL_DATETIME_FORMAT));
        data.insert("Modality", study.modality_);
        data.insert("StudyDesc", study.study_desc_);
        qDebug() << data;
        if (DbManager::insert(study_table_name_, data)) {
            success = true;
        }
    }
    DbManager::CloseDb();
    return success;
}

//----------------------------------------------------------------
bool StudyDao::RemoveStudyFromDb(const QString &study_uid) {
    bool success = false;
    if (study_uid.isEmpty()) {
        return false;
    }
    if(DbManager::OpenDb()) {
        QString where = QString("StudyUid = '%1'").arg(study_uid);
        if (DbManager::remove(study_table_name_, where)) {
            success = true;
        }
    }
    DbManager::CloseDb();
    this->RemoveAllImagesOfStudyFromDb(study_uid, false);
    return success;
}

//----------------------------------------------------------------
/**
 * @brief StudyDao::VerifyStudyByStuid
 * @param study_uid
 * @return
 */
bool StudyDao::VerifyStudyByStuid(const QString &study_uid) {
    bool success = false;
    if (study_uid.isEmpty()) {
        return false;
    }
    if(DbManager::OpenDb()) {
        QStringList key_list;
        key_list.append("StudyUid");
        QString where = QString("StudyUid = '%1'").arg(study_uid);
        QList<QMap<QString, QVariant>> res;
        if (DbManager::select(study_table_name_, key_list, res, where)) {
            if (res.size() == 1) {
                success = true;
            }
        }
    }
    DbManager::CloseDb();
    return success;
}

//----------------------------------------------------------------
bool StudyDao::InsertImageToDb(const ImageRecord &image, bool imported) {
    Q_UNUSED(imported)
    bool success = false;
    if(DbManager::OpenDb()) {
        QMap<QString, QVariant> data;
        data.insert("ImageUid", image.image_uid_);
        data.insert("SopClassUid", image.sop_class_uid_);
        data.insert("SeriesUid", image.series_uid_);
        data.insert("StudyUid", image.study_uid_);
        data.insert("RefImageUid", image.ref_image_uid_);
        data.insert("ImageNo", image.image_number_);
        data.insert("ImageTime", image.image_yime_.toString(NORMAL_DATETIME_FORMAT));
        data.insert("ImageDesc", image.image_desc_);
        data.insert("ImageFile", image.image_file_);
        if (DbManager::insert(image_table_name_, data)) {
            success = true;
        }
    }
    DbManager::CloseDb();
    return success;
}

//----------------------------------------------------------------
bool StudyDao::RemoveImageFromDb(const QString &image_uid, bool updateStudy) {
    Q_UNUSED(updateStudy)
    bool success = false;
    // select data && Remove file
    if (image_uid.isEmpty()) {
        return false;
    }
    if (DbManager::OpenDb()) {
        QStringList key_list;
        key_list.append("ImageFile");
        QString where = QString("ImageUid = '%1'").arg(image_uid);
        QList<QMap<QString, QVariant>> res;
        if (DbManager::select(image_table_name_, key_list, res, where)) {
            if (res.size() == 1) {
                const QMap<QString, QVariant> &res0 = res.at(0);
                if (res0.size() == 1) {
                    QString image_file = res0.value("ImageFile").toString();
                    QString file = QString("./DcmFile/%2").arg(image_file);
                    // QString dir_name = file.left(file.lastIndexOf('/'));
                    FileUtil::DeleteFileOrFolder(file);
                    success = true;
                } else {
                }
            } else {
            }
        }
    }
    DbManager::CloseDb();
    // remove data
    if(DbManager::OpenDb()) {
        QString where = QString("ImageUid = '%1'").arg(image_uid);
        if (DbManager::remove(image_table_name_, where)) {
            success = true;
        }
    }
    DbManager::CloseDb();
    return success;
}

//----------------------------------------------------------------
bool StudyDao::RemoveAllImagesOfStudyFromDb(
    const QString &study_uid, bool updateStudy) {
    Q_UNUSED(updateStudy)
    if (study_uid.isEmpty()) {
        return false;
    }
    bool result = false;
    //
    QStringList image_uids;
    // select data
    if (DbManager::OpenDb()) {
        QStringList key_list;
        key_list.append("ImageUid");
        QString where = QString("StudyUid = '%1'").arg(study_uid);
        QList<QMap<QString, QVariant>> res;
        if (DbManager::select(image_table_name_, key_list, res, where)) {
            if (res.size() >= 1) {
                for (int i = 0; i < res.size(); i++) {
                    const QMap<QString, QVariant> &res0 = res.at(i);
                    if (res0.size() == 1) {
                        image_uids << res0.value("ImageUid").toString();
                    }
                }
            }
        }
    }
    DbManager::CloseDb();
    // remove data
    foreach (auto var, image_uids) {
        RemoveImageFromDb(var);
    }
    return result;
}

//----------------------------------------------------------------
bool StudyDao::UpdateImageFile(const QString &image_uid, const QString &image_file) {
    if (image_uid.isEmpty()) {
        return false;
    }
    if (image_file.isEmpty()) {
        return false;
    }
    bool result = false;
    // Create StudyTable
    QString str ;
    str = "UPDATE ImageTable SET ImageFile=%1 WHERE ImageUid=%2";
    str = str.arg(image_uid, image_file);
    result = DbManager::ExecSqlStr(str);
    return result;
}

//----------------------------------------------------------------
bool StudyDao::VerifyImageByIMmuid(const QString &image_uid) {
    bool success = false;
    if (image_uid.isEmpty()) {
        return false;
    }
    if(DbManager::OpenDb()) {
        QStringList key_list;
        key_list.append("ImageUid");
        QString where = QString("ImageUid = '%1'").arg(image_uid);
        QList<QMap<QString, QVariant>> res;
        if (DbManager::select(image_table_name_, key_list, res, where)) {
            if (res.size() == 1) {
                success = true;
            }
        }
    }
    DbManager::CloseDb();
    return success;
}

//----------------------------------------------------------------
bool StudyDao::Initial() {
    bool result = false;
    if (DbManager::OpenDb()) {
        bool exist;
        if (DbManager::IsExistTable(study_table_name_, exist)) {
            if (!exist) {
                result = CreateTable();
            } else {
                if (CheckTable()) {
                    result = true;
                } else {
                    if (DbManager::RemoveTable(study_table_name_)) {
                        result = CreateTable();
                    }
                }
            }
        }
    }
    DbManager::CloseDb();
    return result;
}

//----------------------------------------------------------------
bool StudyDao::CreateTable() {
    bool result = false;
    // Create StudyTable
    QString str ;
    str = "CREATE TABLE IF NOT EXISTS StudyTable("
          "StudyUid VARCHAR(128) PRIMARY KEY NOT NULL,"
          "AccNumber VARCHAR(64) NOT NULL, PatientId VARCHAR(64) NOT NULL,"
          "PatientName VARCHAR(64), "
          "PatientSex VARCHAR(2) NOT NULL,"
          "PatientBirth DATE NOT NULL,"
          "PatientAge VARCHAR(6),"
          "StudyTime DATETIME NOT NULL,"
          "Modality VARCHAR(2) NOT NULL, "
          "StudyDesc TEXT)";
    result = DbManager::ExecSqlStr(str);
    str = "CREATE INDEX IF NOT EXISTS IX_StudyTable_StudyDate ON StudyTable(StudyTime)";
    result = DbManager::ExecSqlStr(str);
    // Create ImageTable
    str = "CREATE TABLE IF NOT EXISTS ImageTable("
          "ImageUid VARCHAR(128) PRIMARY KEY NOT NULL,"
          "SopClassUid VARCHAR(128) NOT NULL,"
          "SeriesUid VARCHAR(128) NOT NULL, "
          "StudyUid VARCHAR(128) NOT NULL,"
          "RefImageUid VARCHAR(128),"
          "ImageNo VARCHAR(16), "
          "ImageTime DATETIME NOT NULL,"
          "ImageDesc TEXT,"
          "ImageFile TEXT,"
          "FOREIGN KEY(StudyUid) REFERENCES StudyTable(StudyUid))";
    result = DbManager::ExecSqlStr(str);
    str = "CREATE INDEX IF NOT EXISTS IX_ImageTable_ImageTime ON ImageTable(ImageTime)";
    result = DbManager::ExecSqlStr(str);
    return result;
}

//----------------------------------------------------------------
bool StudyDao::CheckTable() {
    bool ok1 = false;
    bool ok2 = false;
    if (DbManager::IsExistTable(study_table_name_, ok1) &&
            DbManager::IsExistTable(image_table_name_, ok2) ) {
        if (ok1 && ok2) {
            return true;
        }
    }
    return false;
}






