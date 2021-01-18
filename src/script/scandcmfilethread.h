#ifndef SCANDCMFILETHREAD_H
#define SCANDCMFILETHREAD_H

#include <QThread>
#include <QStringList>
#include "dcmtk/dcmdata/dcdatset.h"

class StudyRecord;

class ScanDcmFileThread : public QThread {
    Q_OBJECT
  public:
    explicit ScanDcmFileThread(QObject *parent = nullptr);
    void run();
    void SetFiles(const QStringList &files);
    void SetAbort(bool yes);
  Q_SIGNALS:
    void Signal_ResultReady();
    void Signal_ResultRecord(StudyRecord *study);
  private:
    QStringList file_list_;
    bool abort_;

};

#endif // SCANDCMFILETHREAD_H
