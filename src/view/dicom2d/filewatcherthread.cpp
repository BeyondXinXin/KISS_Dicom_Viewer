#include "filewatcherthread.h"

#include <global/KissGlobal>
#include <engine/KissEngine>

//--------------------------------------
FileWatcherThread::FileWatcherThread(const QString &dir, QObject *parent):
    QThread(parent) {
    m_dir_ = dir;
}

//--------------------------------------
void FileWatcherThread::run()  {
    ScanDir();
    exec();
}

//--------------------------------------
void FileWatcherThread::ScanDir() {
    QStringList removed, added;
    removed = m_files_;
    QStringList files = QDir(m_dir_).entryList(QDir::Files);
    foreach (const QString &f, files) {
        QString file = m_dir_ + QDir::separator() + f;
        if (m_files_.contains(file)) {
            removed.removeOne(file);
        } else {
        }
        added += file;
    }
    m_files_ = added;
    emit Signal_FilesChanged(removed, added);
    QTimer::singleShot(500, this, &FileWatcherThread::ScanDir);
}
