#include "global.h"
#include "global/KissDicomViewConfig.h"

#include <dao/KissDb>
#include <engine/KissEngine>


//----------------------------------------------------------------
namespace Kiss {
    QString Sex2TrSex(const QString &sex) {
        if (sex.contains("M", Qt::CaseInsensitive)) {
            return QObject::tr("M");
        } else if (sex.contains("F", Qt::CaseInsensitive)) {
            return QObject::tr("F");
        } else {
            return QObject::tr("O");
        }
    }

    QString TrSex2Sex(const QString &trsex) {
        if (trsex == QObject::tr("M")) {
            return QString("M");
        } else if (trsex == QObject::tr("F")) {
            return QString("F");
        } else {
            return QString("O");
        }
    }

    QTime FormatDicomTime(const QString &timeStr) {
        if (timeStr.contains('.')) {
            return QTime::fromString(timeStr, "hhmmss.zzz");
        } else {
            return QTime::fromString(timeStr, DICOM_TIME_FORMAT);
        }
    }

    void InitialDir() {
#ifdef Q_OS_LINUX
        QString home_path = QStandardPaths::writableLocation(
                                QStandardPaths::HomeLocation);
        if (!FileUtil::DirMake(QString("%1/.KissDicomViewer").arg(home_path))) {
        } else {
            QDir::setCurrent(QString("%1/.KissDicomViewer").arg(home_path));
        }
#endif
        FileUtil::DirMake("./ScpCache/");
        FileUtil::DirMake("./etc/");
        FileUtil::FileCopy(":/xml/modalityprop.xml",
                           "./etc/modalityprop.xml", false);
    }

    void InitialDao() {
        if (!DbManager::DbInitial() ||
                !DbManager::CreateDbFile() ) {
            QMessageBox::critical(nullptr, "Create Log Database",
                                  "Create log database failed: err.");
        } else {
        }
    }

    void InitialStudyDao() {
        if (!StudyDao::Initial()) {
            QMessageBox::critical(nullptr, "Create Log Database",
                                  "Create log database failed: err.");
        } else {
        }
    }

    void InitialLogDao() {
        if (!LogDao::Initial()) {
            QMessageBox::critical(nullptr, "Create Log Database",
                                  "Create log database failed: err.");
        } else {
        }
    }


    bool FileUtil::FileCopy(const QString &src, const QString &dst, const bool cover) {
        if (!QFile::exists(src)) {
            return false;
        }
        if (cover && QFile::exists(dst)) {
            QFile::remove(dst);
        }
        return QFile::copy(src, dst);
    }

    bool FileUtil::FileIsExist(const QString &strFile) {
        QFile tempFile(strFile);
        return tempFile.exists();
    }


    bool FileUtil::DirCopy(const QString &src, const QString &dst) {
        QDir dir(src);
        if (!dir.exists()) {
            return false;
        }
        foreach (QString d, dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
            QString dst_path = dst + QDir::separator() + d;
            FileUtil::DirMake(dst_path);
            FileUtil::DirCopy(src + QDir::separator() + d, dst_path);
        }
        foreach (QString f, dir.entryList(QDir::Files)) {
            QFile::copy(src + QDir::separator() + f, dst + QDir::separator() + f);
        }
        return true;
    }


    bool FileUtil::DirExist(const QString &path) {
        QDir dir(path);
        return dir.exists();
    }


    bool FileUtil::DirMake(const QString &path) {
        QString full_path = GetFullPath(path);
        QDir dir(full_path);
        if (dir.exists()) {
            return true;
        } else {
            return dir.mkpath(full_path);
        }
    }


    bool FileUtil::DirRemove(const QString &path) {
        if (path.isEmpty()) {
            qWarning() << "the path is empty error! ";
            return false;
        }
        QDir dir(path);
        return dir.removeRecursively();
    }

    bool FileUtil::DeleteFileOrFolder(const QString &strPath) {
        if (strPath.isEmpty() || !QDir().exists(strPath)) { //是否传入了空的路径||路径是否存在
            return false;
        }
        QFileInfo FileInfo(strPath);
        if (FileInfo.isFile()) { //如果是文件
            QFile::remove(strPath);
        } else if (FileInfo.isDir()) { //如果是文件夹
            QDir qDir(strPath);
            qDir.removeRecursively();
        }
        return true;
    }

    QString FileUtil::GetFullPath(const QString &path) {
        QFileInfo file_info(path);
        return file_info.absoluteFilePath();
    }

    QString FileUtil::GetFileName(const QString &filter, QString defaultDir) {
        return QFileDialog::getOpenFileName(nullptr, "选择文件", defaultDir, filter);
    }

    QStringList getLocalIP() {
        QStringList ips;
        QList<QHostAddress> addrs = QNetworkInterface::allAddresses();
        foreach (QHostAddress addr, addrs) {
            QString ip = addr.toString();
            if (IsIP(ip)) {
                ips << ip;
            }
        }
        //优先取192开头的IP,如果获取不到IP则取127.0.0.1
        /*QString ip = "127.0.0.1";
        if(ips.size() == 0) {
            ips << ip;
        }*/
        /*foreach (QString str, ips) {
            if (str.startsWith("192.168.1") || str.startsWith("192")) {
                ip = str;
                break;
            }
        }*/
        return ips;
    }

    bool IsIP(const QString &ip) {
        QRegExp RegExp("((2[0-4]\\d|25[0-5]|[01]?\\d\\d?)\\.){3}(2[0-4]\\d|25[0-5]|[01]?\\d\\d?)");
        return RegExp.exactMatch(ip);
    }

    QString GetRandString(const quint32 len, const QString &char_set) {
        QString result("");
        QTime t = QTime::currentTime();
        qsrand(static_cast<quint32>(t.msec() + t.second() * 1000));
        for (quint32 i = 0; i < len; i++) {
            qint32 ir = qrand() % char_set.length();
            result[i] = char_set.at(ir);
        }
        return result;
    }

    void QUIHelper::SetStyle(const QString &qssFile) {
        QFile file(qssFile);
        if (file.open(QFile::ReadOnly)) {
            QString qss;
            QStringList list;
            QTextStream in(&file);
            //in.setCodec("utf-8");
            while (!in.atEnd()) {
                QString line;
                in >> line;
                list << line;
            }
            qss = list.join("\n");
            QString paletteColor = qss.mid(15, 7);
            qApp->setPalette(QPalette(QColor(paletteColor)));
            qApp->setStyleSheet(qss);
            file.close();
        }
    }

    void QUIHelper::SetCode() {
        QTextCodec *codec = QTextCodec::codecForName("utf-8");
        QTextCodec::setCodecForLocale(codec);
    }

}

//----------------------------------------------------------------
LocalSettings::LocalSettings() {
    loadConfig();
}

//----------------------------------------------------------------
void LocalSettings::saveConfig() {
    QFile file(LOCALSETTINGS_CFG);
    if (file.open(QIODevice::WriteOnly)) {
        QDataStream out(&file);
        out << statInfo;
        file.close();
    }
}

//----------------------------------------------------------------
void LocalSettings::loadConfig() {
    QFile file(LOCALSETTINGS_CFG);
    if (file.open(QIODevice::ReadOnly)) {
        QDataStream in(&file);
        in >> statInfo ;
        file.close();
    }
}
