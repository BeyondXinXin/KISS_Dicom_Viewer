#include "sqlimagemodel.h"
#include "../studydao.h"
#include "global/stable.h"

#include "dcmtk/dcmdata/dcuid.h"

//----------------------------------------------------------------
SqlImageModel::SqlImageModel(QObject * parent, QSqlDatabase db)
  : QSqlTableModel(parent, db)
{
    setEditStrategy(QSqlTableModel::OnRowChange);
    setTable("ImageTable");
    setSort(ImageTime, Qt::DescendingOrder);
}

//----------------------------------------------------------------
QVariant SqlImageModel::headerData(int section,
                                   Qt::Orientation orientation, int role) const
{
    if (Qt::DisplayRole == role) {
        if (Qt::Horizontal == orientation) {
            switch (section) {
            case ImageNo:
                return tr("Image No.");
            case ImageTime:
                return tr("Image Time");
            case ImageDesc:
                return tr("Image Desc");
            case ImageFile:
                return tr("Image File");
            default:
                return QSqlTableModel::headerData(section, orientation, role);
            }
        }
    }
    return QSqlTableModel::headerData(section, orientation, role);
}

//----------------------------------------------------------------
QStringList SqlImageModel::getAllImageFiles() const
{
    QStringList files;
    for (int i = 0; i < rowCount(); ++i) {
        QString file = data(index(i, ImageFile)).toString();
        files << file;
    }
    return files;
}

//----------------------------------------------------------------
bool SqlImageModel::select()
{
    bool ret = false;
    ret = QSqlTableModel::select();
    return ret;
}

//----------------------------------------------------------------
void SqlImageModel::SLot_ViewImages(const QModelIndexList & indexes)
{
    QStringList files;
    foreach (QModelIndex idx, indexes) {
        if (idx.column() == ImageFile) {
            files << data(index(idx.row(), ImageFile)).toString();
        }
    }
    emit viewImages(files);
}

//----------------------------------------------------------------
void SqlImageModel::SLot_ViewAllImages()
{
    emit viewImages(getAllImageFiles());
}

//----------------------------------------------------------------
void SqlImageModel::Slot_RemoveImages(const QModelIndexList & indexes)
{
    foreach (QModelIndex idx, indexes) {
        if (idx.column() == ImageFile) {
            StudyDao dao;
            dao.RemoveImageFromDb(
              data(index(idx.row(), ImageUid)).toString());
        }
    }
    emit SgnRemoveFinished();
}

//----------------------------------------------------------------
void SqlImageModel::Slot_RemoveAllImages()
{
    QStringList studyUids;
    for (int i = 0; i < rowCount(); ++i) {
        QString uid = data(index(i, StudyUid)).toString();
        if (!studyUids.contains(uid)) {
            studyUids << uid;
        }
    }
    foreach (QString studyUid, studyUids) {
        StudyDao dao;
        dao.RemoveAllImagesOfStudyFromDb(studyUid);
    }
    emit SgnRemoveFinished();
}

//----------------------------------------------------------------
void SqlImageModel::Slot_StudySelected(const QStringList & studyUids)
{
    bool close = false;
    if (DbManager::IsOpenedDb()) {
    } else {
        if (DbManager::OpenDb()) {
            close = true;
        }
    }
    QString filter;
    if (studyUids.size()) {
        for (int i = 0; i < studyUids.size(); ++i) {
            filter.append(QString("StudyUid=\'%1\'").arg(studyUids.at(i)));
            if (i < studyUids.size() - 1) {
                filter.append(" OR ");
            }
        }
    } else {
        filter.append("StudyUid IS NULL");
    }
    setFilter(filter);
    if (close) {
        DbManager::CloseDb();
    }
}

//----------------------------------------------------------------
void SqlImageModel::SLot_ShowDirectories(const QModelIndexList & indexes)
{
    QStringList files;
    foreach (QModelIndex idx, indexes) {
        if (idx.column() == ImageFile) {
            files << data(index(idx.row(), ImageFile)).toString();
        }
    }
    QFileInfo fileInfo("./DcmFile/" + files.at(0));
    QDesktopServices::openUrl(QUrl(fileInfo.path(), QUrl::TolerantMode));
}
