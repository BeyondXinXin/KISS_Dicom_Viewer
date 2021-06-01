#ifndef SQLIMAGEMODEL_H
#define SQLIMAGEMODEL_H

#include <QSqlTableModel>

class SqlImageModel : public QSqlTableModel
{
    Q_OBJECT
public:
    enum ColumnType
    {
        ImageUid,
        SopClassUid,
        SeriesUid,
        StudyUid,
        RefImageUid,

        ImageNo,
        ImageTime,

        ImageDesc,
        ImageFile,

        ColumnCount,
    };

    explicit SqlImageModel(QObject * parent = nullptr, QSqlDatabase db = QSqlDatabase());

    QVariant headerData(int section, Qt::Orientation orientation = Qt::Horizontal,
                        int role = Qt::DisplayRole) const;

    QStringList getAllImageFiles() const;

Q_SIGNALS:
    void viewImages(const QStringList & imageFiles);
    void Signal_RemoveFinished();
public Q_SLOTS:
    bool select();
public Q_SLOTS:
    void SLot_ViewImages(const QModelIndexList & indexes);
    void SLot_ViewAllImages();
    void Slot_RemoveImages(const QModelIndexList & indexes);
    void Slot_RemoveAllImages();
    void Slot_StudySelected(const QStringList & studyUids);
    void SLot_ShowDirectories(const QModelIndexList & indexes);
};

#endif // SQLIMAGEMODEL_H
