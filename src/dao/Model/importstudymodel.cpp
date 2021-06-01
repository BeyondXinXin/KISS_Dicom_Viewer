#include "importstudymodel.h"
#include "global/global.h"
#include "global/studyrecord.h"

//----------------------------------------------------------------
ImportStudyModel::ImportStudyModel(QObject * parent)
  : QAbstractItemModel(parent)
{
}

//----------------------------------------------------------------
ImportStudyModel::~ImportStudyModel()
{
    qDeleteAll(study_list_);
}

//----------------------------------------------------------------
int ImportStudyModel::rowCount(const QModelIndex & parent) const
{
    if (parent.isValid()) {
        return 0;
    } else {
        return study_list_.size();
    }
}

//----------------------------------------------------------------
int ImportStudyModel::columnCount(const QModelIndex &) const
{
    return ColumnCount;
}

//----------------------------------------------------------------
QModelIndex ImportStudyModel::parent(const QModelIndex &) const
{
    return QModelIndex();
}

//----------------------------------------------------------------
QModelIndex ImportStudyModel::index(int row, int column, const QModelIndex & parent) const
{
    if (parent.isValid() || (row >= study_list_.size()) || (row < 0)
        || (column >= ColumnCount) || (column < 0)) {
        return QModelIndex();
    }
    return createIndex(row, column, study_list_.at(row));
}

//----------------------------------------------------------------
QVariant ImportStudyModel::data(const QModelIndex & index, int role) const
{
    using namespace Kiss;
    if (index.isValid() && (Qt::DisplayRole == role)) {
        int row = index.row();
        switch (index.column()) {
        case AccNumber:
            return study_list_.at(row)->acc_number_;
        case PatientId:
            return study_list_.at(row)->patient_id_;
        case PatientName:
            return study_list_.at(row)->patient_name_;
        case PatientSex:
            return Sex2TrSex(study_list_.at(row)->patient_sex_);
        case PatientBirth:
            return study_list_.at(row)->patient_birth_;
        case StudyTime:
            return study_list_.at(row)->study_time_;
        case Modality:
            return study_list_.at(row)->modality_;
        case Institution:
            return study_list_.at(row)->institution_;
        case Images:
            return study_list_.at(row)->image_list_.size();
        case StudyStatus:
            return study_list_.at(row)->status_;
        default:
            return QVariant();
        }
    } else {
        return QVariant();
    }
}

//----------------------------------------------------------------
QVariant ImportStudyModel::headerData(
  int section, Qt::Orientation orientation, int role) const
{
    if (Qt::DisplayRole == role) {
        if (Qt::Horizontal == orientation) {
            switch (section) {
            case AccNumber:
                return tr("Acc Number");
            case PatientId:
                return tr("Patient Id");
            case PatientName:
                return tr("Patient Name");
            case PatientSex:
                return tr("Sex");
            case PatientBirth:
                return tr("Birthdate");
            case StudyTime:
                return tr("Study Time");
            case Modality:
                return tr("Modality");
            case Institution:
                return tr("Institution");
            case Images:
                return tr("Images");
            case StudyStatus:
                return tr("Status");
            default:
                return QVariant();
            }
        } else {
            return section + 1;
        }
    } else {
        return QVariant();
    }
}

//----------------------------------------------------------------
bool ImportStudyModel::removeRows(int row, int count, const QModelIndex & parent)
{
    if (parent.isValid()) {
        return false;
    }
    beginRemoveRows(parent, row, row + count - 1);
    for (int i = 0; i < count; i++) {
        delete study_list_.takeAt(row);
    }
    endRemoveRows();
    return true;
}

//----------------------------------------------------------------
const QList<StudyRecord *> & ImportStudyModel::getStudyList() const
{
    return study_list_;
}

//----------------------------------------------------------------
void ImportStudyModel::resetStudyStatus(StudyRecord * study)
{
    int row = study_list_.indexOf(study);
    if (row >= 0) {
        QModelIndex index = this->index(row, StudyStatus);
        emit dataChanged(index, index, QVector<int>() << Qt::DisplayRole);
    }
}

//----------------------------------------------------------------
int ImportStudyModel::getFileCount() const
{
    int total = 0;
    foreach (StudyRecord * study, study_list_) {
        total += (study->image_list_.size());
    }
    return total;
}

//----------------------------------------------------------------
QStringList ImportStudyModel::getSelectedStudyUids(const QModelIndexList indexes) const
{
    QStringList studyUids;
    foreach (const QModelIndex & index, indexes) {
        if (index.column() == 0) {
            studyUids << study_list_.at(index.row())->study_uid_;
        }
    }
    return studyUids;
}

//----------------------------------------------------------------
void ImportStudyModel::AppendStudy(StudyRecord * study)
{
    if (study) {
        StudyRecord * s = nullptr;
        foreach (StudyRecord * srec, study_list_) {
            if (srec->study_uid_ == study->study_uid_) {
                s = srec;
                break;
            }
        }
        if (s) {
            s->image_list_.append(study->image_list_);
            study->image_list_.clear();
            delete study;
            int row = study_list_.indexOf(s);
            emit dataChanged(index(row, Images), index(row, Images),
                             QVector<int>() << Qt::DisplayRole);
        } else {
            beginInsertRows(QModelIndex(), rowCount(), rowCount());
            study_list_.append(study);
            endInsertRows();
        }
    }
}

//----------------------------------------------------------------
void ImportStudyModel::AppendStudyList(const QList<StudyRecord *> & studies)
{
    foreach (StudyRecord * study, studies) {
        AppendStudy(study);
    }
}
