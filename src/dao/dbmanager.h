#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QMutex>
#include <QObject>
#include <QSqlDatabase>

namespace Kiss {
class DbManager : public QObject
{
    Q_OBJECT
public:
    enum SQLiteType
    {
        dtNull = 0, //空值类型
        dtInteger = 1, //有符号整数
        dtReal = 2, //有符号浮点数，8字节
        dtText = 3, //文本字符串
        dtBlob = 4, //根据输入类型
        dtVarchar_64 = 5,
        dtTimeStamp = 6,
        dtTimeStamp_NotNull = 7,
    };

public:
    static bool DbInitial();
    static bool Deallocate();
    static bool CreateDbFile();
    static bool IsOpenedDb();
    static bool OpenDb();
    static bool CloseDb();
    static bool IsExistTable(const QString & table_name, bool & result);
    static bool CreateTable(const QString & table_name,
                            const QStringList & key_list,
                            const QList<SQLiteType> & type_list);
    static bool RemoveTable(const QString & table_name);
    static bool IsExistColumn(const QString & table_name,
                              const QString & column_name,
                              bool & result);
    static bool update(const QString & table_name,
                       const QMap<QString, QVariant> & values,
                       const QString & where);
    static bool remove(const QString & table_name,
                       const QString & where = "");
    static bool insert(const QString & table_name,
                       const QMap<QString, QVariant> & values);
    static bool select(const QString & table_name,
                       const QStringList & colunms,
                       QList<QMap<QString, QVariant>> & values,
                       const QString & where = "");
    static bool ExecSqlStr(const QString & sql_str);
signals:
public slots:
private:
    explicit DbManager(QObject * parent = nullptr);
    virtual ~DbManager() override;

public:
    static QSqlDatabase data_base;

private:
    static QMutex file_mutex_;
    static QMutex data_mutex_;
    static QString db_name_;
    static QString file_name_;
    static QStringList sqlite_type_string_;
    static bool init_;
};
} // namespace Kiss

using namespace Kiss;
#endif // DBMANAGER_H
