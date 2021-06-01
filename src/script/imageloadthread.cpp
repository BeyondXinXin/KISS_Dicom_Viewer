#include "imageloadthread.h"

#include <QPointer>
#include <engine/KissEngine>
#include <global/KissGlobal>
#include <view/KissView>

//----------------------------------------------------------------
ImageLoadThread::ImageLoadThread(
  const QStringList & paths, QObject * parent)
  : QThread(parent)
  , m_paths_(paths)
{
}

//----------------------------------------------------------------
void ImageLoadThread::SetImagePaths(
  const QStringList & paths)
{
    m_paths_ = paths;
}

ImageLoadThread::~ImageLoadThread()
{
}

//----------------------------------------------------------------
void ImageLoadThread::run()
{
    QStringList paths = m_paths_;
    foreach (const QString & p, paths) {
        ImageInstance * image = new ImageInstance(p);
        if (image->IsNormal()) {
            emit SgnSignalPathReady(p);
        } else {
        }
        delete image;
    }
}
