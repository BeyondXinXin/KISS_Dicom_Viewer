#include "dbmanager.h"

#include <global/KissGlobal>
#include <engine/KissEngine>

//----------------------------------------------------------------
QSqlDatabase DbManager::data_base;
QMutex DbManager::file_mutex_;
QMutex DbManager::data_mutex_;
QString DbManager::db_name_ = DB_CONNECTION_NAME;
QString DbManager::file_name_ = DB_NAME;
QStringList DbManager::sqlite_type_string_;
bool DbManager::init_ = false;

//----------------------------------------------------------------
bool DbManager::DbInitial() {
    QMutexLocker locker(&data_mutex_);
    if (!init_) {
        init_ = true;
        if (QSqlDatabase::contains(db_name_)) {
            data_base = QSqlDatabase::database(db_name_);
        } else {
            data_base = QSqlDatabase::addDatabase("QSQLITE", db_name_);
        }
        sqlite_type_string_.append("NULL");
        sqlite_type_string_.append("INTEGER");
        sqlite_type_string_.append("REAL");
        sqlite_type_string_.append("TEXT");
        sqlite_type_string_.append("BLOB");
        sqlite_type_string_.append("VARCHAR ( 64 )");
        sqlite_type_string_.append("TimeStamp");
        sqlite_type_string_.append("TimeStamp NOT NULL");
        return true;
    }
    return true;
}

//----------------------------------------------------------------
bool DbManager::Deallocate() {
    QMutexLocker locker(&data_mutex_);
    data_base = QSqlDatabase();
    if (QSqlDatabase::contains(db_name_)) {
        QSqlDatabase::removeDatabase(db_name_);
    }
    return true;
}

//----------------------------------------------------------------
bool DbManager::CreateDbFile() {
    if (!QFile::exists(file_name_)) {
        QFile db_file(file_name_);
        if (!db_file.open(QIODevice::WriteOnly)) {
            db_file.close();
            qWarning() << "dbFile open failed";
            return false;
        }
        db_file.close();
    }
    return true;
}

//----------------------------------------------------------------
bool DbManager::IsOpenedDb() {
    QMutexLocker locker(&data_mutex_);
    return data_base.isOpen();
}

//----------------------------------------------------------------
bool DbManager::OpenDb() {
    file_mutex_.lock();
    if (!IsOpenedDb()) {
        QMutexLocker locker(&data_mutex_);
        data_base.setDatabaseName(file_name_);
        if (!data_base.open()) {
            qWarning() << "database open error:" << data_base.lastError().text();
            return false;
        }
    }
    return true;
}

//----------------------------------------------------------------
bool DbManager::CloseDb() {
    file_mutex_.unlock();
    if (IsOpenedDb()) {
        QMutexLocker locker(&data_mutex_);
        data_base.close();
    }
    return true;
}

//----------------------------------------------------------------
bool DbManager::IsExistTable(const QString &table_name, bool &result) {
    if (!IsOpenedDb()) {
        qWarning() << "database not open error!";
        return false;
    }
    QString sql_str = QString("SELECT 1 FROM sqlite_master "
                              "WHERE type = 'table' AND  "
                              "name = '%1'").arg(table_name);
    QMutexLocker locker(&data_mutex_);
    QSqlQuery query(data_base);
    if (query.exec(sql_str)) {
        if (query.next()) {
            qint32 sql_result = query.value(0).toInt(); //有表时返回1，无表时返回null
            if (sql_result) {
                result = true;
                return true;
            } else {
                result = false;
                return true;
            }
        } else {
            result = false;
            return true;
        }
    } else {
        qWarning() << "sqlstr exec error:" << data_base.lastError().text();
        return false;
    }
}

//----------------------------------------------------------------
bool DbManager::CreateTable(const QString &table_name,
                            const QStringList &key_list,
                            const QList<DbManager::SQLiteType> &type_list) {
    if (key_list.size() != type_list.size()) {
        qWarning() << "keylist != typelist error";
        return false;
    }
    if (!IsOpenedDb()) {
        qWarning() << "database not open error!";
        return false;
    }
    QString sql_str_1 = QString("CREATE TABLE %1 (").arg(table_name);
    QString sql_str_2 = "%1 %2 PRIMARY KEY ,";
    QString sql_str_temp = "%1 %2 ,";
    sql_str_2 = sql_str_2
                .arg(key_list.at(0))
                .arg(sqlite_type_string_.at(type_list.at(0)));
    for (qint32 i = 1; i < type_list.size(); ++i) {
        sql_str_2 += sql_str_temp.arg(key_list.at(i))
                     .arg(sqlite_type_string_.at(type_list.at(i)));
    }
    sql_str_2 = sql_str_2.left(sql_str_2.size() - 1);
    QString sql_str_3 = ");";
    QString sql_str = sql_str_1 + sql_str_2 + sql_str_3;
    QMutexLocker locker(&data_mutex_);
    QSqlQuery query(data_base);
    if (query.exec(sql_str)) {
        return true;
    } else {
        qWarning() << "sqlstr exec error:" << data_base.lastError().text();
        return false;
    }
}

//----------------------------------------------------------------
bool DbManager::RemoveTable(const QString &table_name) {
    if (!IsOpenedDb()) {
        qWarning() << "database not open error!";
        return false;
    }
    QString sql_str = QString("DROP TABLE '%1'").arg(table_name);
    QMutexLocker locker(&data_mutex_);
    QSqlQuery query(data_base);
    if (query.exec(sql_str)) {
        return true;
    } else {
        qWarning() << "sqlstr exec error:" << data_base.lastError().text();
        return false;
    }
}

//----------------------------------------------------------------
bool DbManager::IsExistColumn(const QString &table_name,
                              const QString &column_name,
                              bool &result) {
    if (!IsOpenedDb()) {
        qWarning() << "database not open error!";
        return false;
    }
    QString sql_str = QString("SELECT 1 FROM sqlite_master "
                              "WHERE type = 'table' and "
                              "name = '%1' and sql like '%%2%' "
                             ).arg(table_name).arg(column_name);
    QMutexLocker locker(&data_mutex_);
    QSqlQuery query(data_base);
    if (query.exec(sql_str)) {
        if (query.next()) {
            qint32 sql_result = query.value(0).toInt(); //有此字段时返回1，无字段时返回null
            if (sql_result) {
                result = true;
                return true;
            } else {
                result = false;
                return true;
            }
        } else {
            result = false;
            return true;
        }
    } else {
        qWarning() << "sqlstr exec error:" << data_base.lastError().text();
        return false;
    }
}

//----------------------------------------------------------------
bool DbManager::update(const QString &table_name,
                       const QMap<QString, QVariant> &values,
                       const QString &where) {
    if (!IsOpenedDb()) {
        qWarning() << "database not open error!";
        return false;
    }
    QString sql_str_data;
    QList<QString> key_list = values.keys();
    foreach (QString key, key_list) {
        if (!sql_str_data.isEmpty()) {
            sql_str_data += ",";
        }
        sql_str_data += QString("%1=?").arg(key);
    }
    QString sql_str;
    if (where.isEmpty()) {
        sql_str = QString("UPDATE %1 SET %2"
                         ).arg(table_name).arg(sql_str_data);
    } else {
        sql_str = QString("UPDATE %1 SET %2 WHERE %3"
                         ).arg(table_name).arg(sql_str_data).arg(where);
    }
    QMutexLocker locker(&data_mutex_);
    QSqlQuery query(data_base);
    query.prepare(sql_str);
    for (qint32 i = 0; i < key_list.count(); ++i) {
        query.bindValue(i, values.value(key_list.at(i)));
    }
    if (query.exec()) {
        return true;
    } else {
        qWarning() << "sqlstr exec error:" << data_base.lastError().text();
        return false;
    }
}

//----------------------------------------------------------------
bool DbManager::remove(const QString &table_name,
                       const QString &where) {
    if (!IsOpenedDb()) {
        qWarning() << "database not open error!";
        return false;
    }
    QMutexLocker locker(&data_mutex_);
    QString sql_str = QString("DELETE FROM %1 WHERE %2"
                             ).arg(table_name).arg(where);
    QSqlQuery query(data_base);
    if (query.exec(sql_str)) {
        return true;
    } else {
        qWarning() << "sqlstr exec error:" << data_base.lastError().text();
        return false;
    }
}

//----------------------------------------------------------------
bool DbManager::insert(const QString &table_name,
                       const QMap<QString, QVariant> &values) {
    if (!IsOpenedDb()) {
        qWarning() << "database not open error!";
        return false;
    }
    QString sql_str_column, sql_str_data;
    QList<QString> key_list = values.keys();
    foreach (QString key, key_list) {
        if (!sql_str_column.isEmpty()) {
            sql_str_column += ",";
        }
        sql_str_column += key;
        if (!sql_str_data.isEmpty()) {
            sql_str_data += ",";
        }
        sql_str_data += "?";
    }
    QString sql_str = QString("INSERT INTO %1(%2) VALUES(%3)")
                      .arg(table_name).arg(sql_str_column).arg(sql_str_data);
    QMutexLocker locker(&data_mutex_);
    QSqlQuery query(data_base);
    query.prepare(sql_str);
    for (qint32 i = 0; i < key_list.count(); ++i) {
        if(values.value(key_list.at(i)).toString().isEmpty()) {
            query.bindValue(i, "--");
        } else {
            query.bindValue(i, values.value(key_list.at(i) ));
        }
    }
    if (query.exec()) {
        return true;
    } else {
        qWarning() << "sqlstr exec error:" << data_base.lastError().text();
        return false;
    }
}

//----------------------------------------------------------------
bool DbManager::select(const QString &table_name,
                       const QStringList &colunms,
                       QList<QMap<QString, QVariant>> &values,
                       const QString &where) {
    if (!IsOpenedDb()) {
        qWarning() << "database not open error!";
        return false;
    }
    QString sql_str_columns;
    if (colunms.size()) {
        sql_str_columns = colunms.join(",");
    } else {
//        sql_str_columns = "*";
        qWarning() << "colunms is null";
        return false;
    }
    QString sql_str;
    if (where.isEmpty()) {
        sql_str = QString("SELECT %1 FROM %2")
                  .arg(sql_str_columns)
                  .arg(table_name);
    } else {
        sql_str = QString("SELECT %1 FROM %2 WHERE %3")
                  .arg(sql_str_columns)
                  .arg(table_name).arg(where);
    }
    QMutexLocker locker(&data_mutex_);
    QSqlQuery query(data_base);
    if (query.exec(sql_str)) {
        qint32 columns_sum = query.record().count();
        while (query.next()) {
            QMap<QString, QVariant> row;
            for (qint32 i = 0; i < columns_sum; ++i) {
                row.insert(colunms.at(i), query.value(i));
            }
            values.append(row);
        }
        return true;
    } else {
        qWarning() << "sqlstr exec error:" << data_base.lastError();
        return false;
    }
}

//----------------------------------------------------------------
bool DbManager::ExecSqlStr(const QString &sql_str) {
    if (!IsOpenedDb()) {
        qWarning() << "database not open error!";
        return false;
    }
    QMutexLocker locker(&data_mutex_);
    QSqlQuery query(data_base);
    if (query.exec(sql_str)) {
        return true;
    } else {
        qWarning() << "sqlstr exec error:" << data_base.lastError().text();
        return false;
    }
}

//----------------------------------------------------------------
DbManager::DbManager(QObject *parent) : QObject(parent) {
}

//----------------------------------------------------------------
DbManager::~DbManager() {
}

