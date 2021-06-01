#include "logdbmanager.h"

#include <global/KissGlobal>

//----------------------------------------------------------------
const QString LogDao::kTableName = "LogTable";

//----------------------------------------------------------------
const QStringList LogDao::string_tables_ =
  QStringList()
  << QObject::tr("All Type") << QObject::tr("System Info") << QObject::tr("System Warn") << QObject::tr("System Error");

//----------------------------------------------------------------
LogDao::LogDao(QObject * parent)
  : QObject(parent)
{
}

//----------------------------------------------------------------
LogDao::~LogDao()
{
}

//----------------------------------------------------------------
bool LogDao::InsertMessageToDao(
  const QString & name, const LogDao::EventType & type, const QString & msg)
{
    bool success = false;
    if (DbManager::OpenDb()) {
        QMap<QString, QVariant> data;
        data.insert("UserName", name);
        if (type < ET_TypeCount) {
            data.insert("EventType", string_tables_[type]);
            data.insert("EventContent", msg);
        } else {
            data.insert("EventType", string_tables_[ET_SysError]);
            data.insert("EventContent", QObject::tr("Error while logging message: %1.").arg(msg));
        }
        QDateTime current_date_time = QDateTime::currentDateTime();
        data.insert("LogTime", current_date_time.toString("yyyy-MM-dd hh:mm:ss.zzz"));
        if (DbManager::insert(kTableName, data)) {
            success = true;
        }
    }
    DbManager::CloseDb();
    return success;
}

//----------------------------------------------------------------
bool LogDao::Initial()
{
    bool result = false;
    if (DbManager::OpenDb()) {
        bool exist;
        if (DbManager::IsExistTable(kTableName, exist)) {
            if (!exist) {
                result = CreateTable();
            } else {
                if (CheckTable()) {
                    result = true;
                } else {
                    if (DbManager::RemoveTable(kTableName)) {
                        result = CreateTable();
                    }
                }
            }
        }
    }
    DbManager::CloseDb();
    return result;
}

//----------------------------------------------------------------
bool LogDao::CreateTable()
{
    QStringList key_list;
    key_list.append("LogTime");
    key_list.append("UserName");
    key_list.append("EventType");
    key_list.append("EventContent");
    QList<DbManager::SQLiteType> type_list;
    type_list.append(DbManager::dtTimeStamp_NotNull);
    type_list.append(DbManager::dtVarchar_64);
    type_list.append(DbManager::dtVarchar_64);
    type_list.append(DbManager::dtText);
    return DbManager::CreateTable(kTableName, key_list, type_list);
}

//----------------------------------------------------------------
bool LogDao::CheckTable()
{
    bool ok1 = false;
    bool ok2 = false;
    bool ok3 = false;
    bool ok4 = false;
    if (DbManager::IsExistColumn(kTableName, "LogTime", ok1) && DbManager::IsExistColumn(kTableName, "UserName", ok2) && DbManager::IsExistColumn(kTableName, "EventType", ok3) && DbManager::IsExistColumn(kTableName, "EventContent", ok4)) {
        if (ok1 && ok2 && ok3 && ok4) {
            return true;
        }
    }
    return false;
}
