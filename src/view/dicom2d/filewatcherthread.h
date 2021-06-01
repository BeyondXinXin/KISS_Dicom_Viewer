#ifndef FILEWATCHERTHREAD_H
#define FILEWATCHERTHREAD_H

#include <QThread>

class FileWatcherThread : public QThread
{
    Q_OBJECT
public:
    FileWatcherThread(const QString & dir, QObject * parent = nullptr);
    void run() Q_DECL_OVERRIDE;
Q_SIGNALS:
    void Signal_FilesChanged(const QStringList & removed, const QStringList & added);

private:
    void ScanDir();

private:
    QString m_dir_;
    QStringList m_files_;
};

#endif // FILEWATCHERTHREAD_H
