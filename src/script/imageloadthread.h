#ifndef IMAGELOADTHREAD_H
#define IMAGELOADTHREAD_H

#include <QStringList>
#include <QThread>
class ImageInstance;

class ImageLoadThread : public QThread
{
    Q_OBJECT
public:
    explicit ImageLoadThread(
      const QStringList & paths = QStringList(),
      QObject * parent = nullptr);

    void SetImagePaths(const QStringList & paths);
    virtual ~ImageLoadThread();
    void run();

signals:
    void ResultReady(ImageInstance * image);
    void SignalPathReady(const QString path);
    void Signal_ImageLoadFinished();

public:
private:
    QStringList m_paths_;
};

#endif // IMAGELOADTHREAD_H
