#ifndef PLUGANINTERFACE_H
#define PLUGANINTERFACE_H

#include <QtPlugin>
#include "stable.h"
#include "stdafx.h"

QT_BEGIN_NAMESPACE
class QImage;
class QPainter;
class QWidget;
class QPainterPath;
class QPoint;
class QRect;
class QString;
class QStringList;
QT_END_NAMESPACE

using Pretreatmen = std::function <QPixmap(const QPixmap &, QWidget *) >;

class PretreatmentInterface {
  public:
    virtual ~PretreatmentInterface() {}

    virtual QStringList GetPretreatments() const = 0;
    virtual Pretreatmen PretreatmentFun(const QString &str) = 0;
};

QT_BEGIN_NAMESPACE
#define PretreatmentInterface_iid "kissdicomviewer_PretreatmentInterface"
Q_DECLARE_INTERFACE(PretreatmentInterface, PretreatmentInterface_iid)
QT_END_NAMESPACE


#endif // PLUGANINTERFACE_H
