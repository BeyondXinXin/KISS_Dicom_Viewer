#ifndef STUDYRECORD_H
#define STUDYRECORD_H

#include <QList>
#include <QString>
#include <QDateTime>



class StudyRecord;
class ImageRecord {
  public:
    ImageRecord(const QString &uid = QString()):
        image_uid_(uid) {}
    QString image_uid_;
    QString sop_class_uid_;
    QString series_uid_;
    QString study_uid_;
    QString ref_image_uid_;
    QString image_number_;
    QDateTime image_yime_;
    QString image_desc_;
    QString image_file_;
};

class StudyRecord {
  public:
    StudyRecord(const QString &uid = QString()) :
        study_uid_(uid) {}
    ~StudyRecord() {
        qDeleteAll(image_list_);
    }
    QString study_uid_;
    QString acc_number_;
    QString patient_id_;
    QString patient_name_;
    QString patient_sex_;
    QDate patient_birth_;
    QString patient_age_;
    QDateTime study_time_;
    QString modality_;
    QString institution_;
    QString status_;
    QString study_desc_;
    QList<ImageRecord *> image_list_;
};

#endif // STUDYRECORD_H

