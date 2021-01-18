#ifndef STUDYDAO_H
#define STUDYDAO_H

#include "dao/dbmanager.h"

class StudyRecord;
class ImageRecord;

class StudyDao : public QObject {
    Q_OBJECT
  public:
    static const QString study_table_name_;
    static const QString image_table_name_;
  public:
    explicit StudyDao(QObject *parent = nullptr);
    virtual ~StudyDao() override;
    bool InsertStudyToDb(const StudyRecord &study, bool imported = false);
    bool RemoveStudyFromDb(const QString &study_uid);
    bool VerifyStudyByStuid(const QString &study_uid);
    //
    bool InsertImageToDb(const ImageRecord &image, bool imported = false);
    bool RemoveImageFromDb(const QString &image_uid, bool updateStudy = true);
    bool RemoveAllImagesOfStudyFromDb(const QString &study_uid, bool updateStudy = true);
    bool UpdateImageFile(const QString &image_uid, const QString &image_file);
    bool VerifyImageByIMmuid(const QString &image_uid);
  public:
    static bool Initial();
  private:
    static bool CreateTable();
    static bool CheckTable();
  private:
};

#endif // STUDYDAO_H
