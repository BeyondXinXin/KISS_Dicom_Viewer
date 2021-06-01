#ifndef MODALITYPROPERTY_H
#define MODALITYPROPERTY_H

#include "dcmtk/dcmdata/dctagkey.h"
#include <QList>
#include <QString>
#include <QXmlStreamReader>

#include <global/KissGlobal>

typedef struct AnnoItem
{
    QString text;
    QList<DcmTagKey> keys;
} AnnoItem;

typedef struct AnnoItemGroup
{
    QString pos;
    QList<AnnoItem *> items;
    AnnoItemGroup()
    {
    }
    ~AnnoItemGroup()
    {
        qDeleteAll(items);
    }

private:
    Q_DISABLE_COPY(AnnoItemGroup)
} AnnoItemGroup;

typedef struct ModalityPref
{
    double adjust_factor;
    double zoom_factor;
    double zoom_max;
    double zoom_min;
    double magnifier_inc;
    double magnifier_max;
    double magnifier_min;
    ModalityPref()
      : adjust_factor(16.0)
      , zoom_factor(0.02)
      , zoom_max(32.0)
      , zoom_min(0.05)
      , magnifier_inc(1.0)
      , magnifier_max(32.0)
      , magnifier_min(2.0)
    {
    }
} ModalityPref;

typedef struct ModalityProp
{
    QString name;
    QList<AnnoItemGroup *> groups;
    ModalityPref pref;
    ModalityProp()
    {
    }
    ~ModalityProp()
    {
        qDeleteAll(groups);
    }

private:
    Q_DISABLE_COPY(ModalityProp)
} ModalityProp;

class ModalityProperty
{
public:
    static ModalityProperty * Instance();
    ModalityProperty();
    ~ModalityProperty();
    const ModalityProp * getModalityProp(const QString & modality) const;
    bool IsNormal() const;
    QString ErrorStr() const;
    void ReadProperty();

private:
    void ReadModality(ModalityProp & m);
    void ReadPref(ModalityPref & p);
    void ReadAnnoGroup(AnnoItemGroup & g);

private:
    static ModalityProperty * instance;
    QXmlStreamReader m_xml_;
    QList<ModalityProp *> m_props_;
    Q_DISABLE_COPY(ModalityProperty)
};

#endif // MODALITYPROPERTY_H
