#ifndef GLOBAL_H
#define GLOBAL_H

#include <QDate>
#include <QTime>
#include <QFile>
#include <QPointer>
#include <QSettings>
#include <QStringList>
#include <QDataStream>
#include <QCoreApplication>

#ifndef GLOBAL_CPP
    #define GLOBAL_EXTERN extern
#else
    #define GLOBAL_EXTERN
#endif

namespace Kiss {
    GLOBAL_EXTERN QString Sex2TrSex(const QString &sex);
    GLOBAL_EXTERN QString TrSex2Sex(const QString &trsex);
    GLOBAL_EXTERN QTime FormatDicomTime(const QString &timeStr);

    GLOBAL_EXTERN bool IsIP(const QString &ip);
    GLOBAL_EXTERN QStringList getLocalIP();

    GLOBAL_EXTERN void InitialDir();
    GLOBAL_EXTERN void InitialDao();
    GLOBAL_EXTERN void InitialLogDao();
    GLOBAL_EXTERN void InitialStudyDao();

    GLOBAL_EXTERN QString GetRandString(
        const quint32 len = 6,
        const QString &char_set =
            QString("0123456789"
                    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                    "abcdefghijklmnopqrstuvwxyz"));



}
namespace Kiss {
    class QUIHelper : public QObject {
        Q_OBJECT
      public:
        // 设置全局样式
        static void SetStyle(const QString &qssFile = ":/Style/style.qss");

        // 设置编码
        static void SetCode();
    };
}

namespace Kiss {
    class FileUtil : public QFile {
      public:
        static bool FileCopy(const QString &src,
                             const QString &dst,
                             const bool cover = false);
        static bool FileIsExist(const QString &strFile);
        static bool DirCopy(const QString &src, const QString &dst);
        static bool DirExist(const QString &path);
        static bool DirMake(const QString &path);
        static bool DirRemove(const QString &path);
        static bool DeleteFileOrFolder(const QString &strPath);
        static QString GetFullPath(const QString &path);
        static QString GetFileName(const QString &filter,
                                   QString defaultDir =
                                       QCoreApplication::applicationDirPath());
    };
}

struct StationInfo {
    QString aetitle;
    ushort store_port;
    friend QDataStream &operator<<(QDataStream &out, const StationInfo &info) {
        return out << info.aetitle << info.store_port;
    }
    friend QDataStream &operator>>(QDataStream &in, StationInfo &info) {
        return in >> info.aetitle >> info.store_port;
    }
};

class LocalSettings {
  public:
    LocalSettings();
    void saveConfig();
    void loadConfig();
    StationInfo statInfo;
};

#endif // GLOBAL_H
