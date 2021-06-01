#ifndef SQLSTUDYMODEL_H
#define SQLSTUDYMODEL_H

#include <QSqlRecord>
#include <QSqlTableModel>

class StudyRecord;
class QItemSelection;

class SqlStudyModel : public QSqlTableModel
{
    Q_OBJECT
public:
    enum ColumnType
    {
        StudyUid,
        AccNumber,
        PatientId,
        PatientName,
        PatientSex,
        PatientBirth,
        PatientAge,
        StudyTime,
        Modality,
        StudyDesc,
        //        ColumnCount,
    };

    explicit SqlStudyModel(QObject * parent = nullptr,
                           QSqlDatabase db = QSqlDatabase());

    QVariant headerData(int section,
                        Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    QString getFirstSelectedStudyUid() const;

public Q_SLOTS:
    bool select();

Q_SIGNALS:
    void SgnStudySelectionChanged(const QStringList & studyUids);
    void SgnNewStudy(const QSqlRecord & studyRec);
    void SgnNewImage(const QSqlRecord & studyRec);
    void SgnRemoveFinished();

public Q_SLOTS:
    void Slot_SelectionChanged(const QModelIndexList & indexes);
    void Slot_RemoveStudies();
    void Slot_NewStudy(const QModelIndex & index);
    void Slot_NewImage(const QModelIndex & index);

private:
    QStringList selected_study_uids_;
    StudyRecord * mod_study_;
    int modify_row_;
};

#endif // SQLSTUDYMODEL_H
