#include "sqlstudymodel.h"

#include "../studydao.h"
#include <global/KissGlobal>

//----------------------------------------------------------------
SqlStudyModel::SqlStudyModel(QObject *parent, QSqlDatabase db) :
    QSqlTableModel(parent, db) {
    setEditStrategy(QSqlTableModel::OnRowChange);
    modify_row_ = -1;
    setTable("StudyTable");
    setSort(StudyTime, Qt::DescendingOrder);
}

//----------------------------------------------------------------
QVariant SqlStudyModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (Qt::DisplayRole == role) {
        if (Qt::Horizontal == orientation) {
            switch (section) {
                case AccNumber:
                    return tr("Acc Number");
                case PatientId:
                    return tr("Patient ID");
                case PatientName:
                    return tr("Name");
                case PatientSex:
                    return tr("Sex");
                case PatientBirth:
                    return tr("Birthdate");
                case PatientAge:
                    return tr("Age");
                case StudyTime:
                    return tr("Study Time");
                case Modality:
                    return tr("Modality");
                case StudyDesc:
                    return tr("Study Desc");
                default:
                    return QSqlTableModel::headerData(section, orientation, role);
            }
        }
    }
    return QSqlTableModel::headerData(section, orientation, role);
}

//----------------------------------------------------------------
QVariant SqlStudyModel::data(const QModelIndex &index, int role) const {
    if (index.isValid()) {
        if (Qt::DisplayRole == role) {
            switch (index.column()) {
                case PatientSex:
                    return Sex2TrSex(QSqlTableModel::data(index, role).toString());
                case PatientAge: {
                        QString ageStr = QSqlTableModel::data(index, role).toString();
                        QString ageUnit = ageStr.right(1);
                        if ((ageUnit == "Y") || (ageUnit == "y")) {
                            return QString("%1%2").arg(ageStr.left(ageStr.size() - 1), tr("Years"));
                        } else if ((ageUnit == "M") || (ageUnit == "m")) {
                            return QString("%1%2").arg(ageStr.left(ageStr.size() - 1), tr("Months"));
                        } else if ((ageUnit == "W") || (ageUnit == "w")) {
                            return QString("%1%2").arg(ageStr.left(ageStr.size() - 1), tr("Weeks"));
                        } else if ((ageUnit == "D") || (ageUnit == "d")) {
                            return QString("%1%2").arg(ageStr.left(ageStr.size() - 1), tr("Days"));
                        } else {
                            return ageStr;
                        }
                    }
                default:
                    return QSqlTableModel::data(index, role);
            }
        } else if (Qt::TextColorRole == role) {
            return QSqlTableModel::data(index, role);
        }
    }
    return QSqlTableModel::data(index, role);
}

//----------------------------------------------------------------
QString SqlStudyModel::getFirstSelectedStudyUid() const {
    if (selected_study_uids_.size()) {
        return selected_study_uids_.first();
    } else {
        return QString();
    }
}

//----------------------------------------------------------------
void SqlStudyModel::Slot_SelectionChanged(const QModelIndexList &indexes) {
    selected_study_uids_.clear();
    foreach (const QModelIndex &idx, indexes) {
        if (idx.column() == AccNumber) {
            selected_study_uids_ << data(index(idx.row(), StudyUid)).toString();
        }
    }
    emit Signal_studySelectionChanged(selected_study_uids_);
}

//----------------------------------------------------------------
void SqlStudyModel::Slot_RemoveStudies() {
    foreach (QString uid, selected_study_uids_) {
        StudyDao dao;
        dao.RemoveStudyFromDb(uid);
    }
    emit Signal_RemoveFinished();
}

//----------------------------------------------------------------
bool SqlStudyModel::select() {
    selected_study_uids_.clear();
    emit Signal_studySelectionChanged(selected_study_uids_);
    bool ret = false;
    ret = QSqlTableModel::select();
    return ret;
}

//----------------------------------------------------------------
void SqlStudyModel::Slot_NewStudy(const QModelIndex &index) {
    emit Signal_NewStudy(record(index.row()));
}

//----------------------------------------------------------------
void SqlStudyModel::Slot_NewImage(const QModelIndex &index) {
    emit Signal_NewImage(record(index.row()));
}
