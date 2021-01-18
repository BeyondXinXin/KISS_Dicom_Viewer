#ifndef LOGDBMANAGER_H
#define LOGDBMANAGER_H

#include "dao/dbmanager.h"


class LogDao : public QObject {
    Q_OBJECT
  public:
    enum EventType {
        ET_AllType,

        ET_SysInfo,
        ET_SysWarn,
        ET_SysError,

        ET_TypeCount,
    };
    static const QString kTableName;
    static const QStringList string_tables_;
  public:
    explicit LogDao(QObject *parent = nullptr);
    virtual ~LogDao() override;
    bool InsertMessageToDao(
        const QString &name, const EventType &type, const QString &msg);
  public:
    static bool Initial();
  private:
    static bool CreateTable();
    static bool CheckTable();
  private:
};



#endif // LOGDBMANAGER_H
