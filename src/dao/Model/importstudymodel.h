#ifndef IMPORTSTUDYMODEL_H
#define IMPORTSTUDYMODEL_H

#include <QAbstractItemModel>
#include <QSqlRecord>
class StudyRecord;

class ImportStudyModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    enum ColumnType
    {

        AccNumber,
        PatientId,
        PatientName,
        PatientSex,
        PatientBirth,

        StudyTime,
        Modality,
        Institution,

        Images,
        StudyStatus,

        ColumnCount,
    };

    explicit ImportStudyModel(QObject * parent = nullptr);
    ~ImportStudyModel();

    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    int columnCount(const QModelIndex & /*parent*/) const;
    QModelIndex parent(const QModelIndex & /*child*/) const;
    QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    bool removeRows(int row, int count, const QModelIndex & parent = QModelIndex());

    const QList<StudyRecord *> & getStudyList() const;
    void resetStudyStatus(StudyRecord * study);
    int getFileCount() const;
    QStringList getSelectedStudyUids(const QModelIndexList indexes) const;

public Q_SLOTS:
    void AppendStudy(StudyRecord * study);
    void AppendStudyList(const QList<StudyRecord *> & studies);

private:
    QList<StudyRecord *> study_list_;
};

#endif // IMPORTSTUDYMODEL_H
