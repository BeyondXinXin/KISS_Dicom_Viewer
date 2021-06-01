#ifndef IMPORTDCMFILETHREAD_H
#define IMPORTDCMFILETHREAD_H

#include <QMap>
#include <QThread>
class ImportStudyModel;
class ImageRecord;
class ReportRecord;

class ImportDcmFileThread : public QThread
{
    Q_OBJECT
public:
    explicit ImportDcmFileThread(ImportStudyModel * model, QObject * parent = nullptr);
    void run();
Q_SIGNALS:
    void SgnResultReady();

public Q_SLOTS:
    void SetAbort(bool yes);

private:
    volatile bool abort_;
    ImportStudyModel * import_model_;
};

#endif // IMPORTDCMFILETHREAD_H
