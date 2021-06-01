#include "modalityproperty.h"

#include <engine/KissEngine>
#include <global/KissGlobal>

//-------------------------------------------------------
ModalityProperty * ModalityProperty::instance = nullptr;
ModalityProperty * ModalityProperty::Instance()
{
    if (!instance) {
        static QMutex mutex;
        QMutexLocker locker(&mutex);
        if (!instance) {
            instance = new ModalityProperty;
        }
    }
    return instance;
}

//-------------------------------------------------------
ModalityProperty::ModalityProperty()
{
    ReadProperty();
}

//-------------------------------------------------------
ModalityProperty::~ModalityProperty()
{
    qDeleteAll(m_props_);
}

//-------------------------------------------------------
const ModalityProp * ModalityProperty::getModalityProp(const QString & modality) const
{
    foreach (const ModalityProp * p, m_props_) {
        if (p->name == modality) {
            return p;
        }
    }
    return nullptr;
}

//-------------------------------------------------------
bool ModalityProperty::IsNormal() const
{
    return m_xml_.error() == QXmlStreamReader::NoError;
}

//-------------------------------------------------------
QString ModalityProperty::ErrorStr() const
{
    return QObject::tr("%1\nLine %2, column %3")
      .arg(m_xml_.errorString())
      .arg(m_xml_.lineNumber())
      .arg(m_xml_.columnNumber());
}

//-------------------------------------------------------
void ModalityProperty::ReadProperty()
{
    QFile f("./etc/modalityprop.xml");
    f.open(QIODevice::ReadOnly);
    m_xml_.setDevice(&f);
    if (m_xml_.readNextStartElement()) {
        if (m_xml_.name() == "modalityProp") {
            while (m_xml_.readNextStartElement()) {
                if (m_xml_.name() == "modalities") {
                    while (m_xml_.readNextStartElement()) {
                        if (m_xml_.name() == "modality") {
                            ModalityProp * m = new ModalityProp;
                            ReadModality(*m);
                            if (m->name.isEmpty()) {
                                delete m;
                            } else {
                                m_props_ << m;
                            }
                        } else {
                            m_xml_.skipCurrentElement();
                        }
                    }
                } else {
                    m_xml_.skipCurrentElement();
                }
            }
        } else {
            m_xml_.raiseError(QObject::tr("Invalid modality property xml file."));
        }
    }
}

//-------------------------------------------------------
void ModalityProperty::ReadModality(ModalityProp & m)
{
    Q_ASSERT(m_xml_.isStartElement() && m_xml_.name() == "modality");
    while (m_xml_.readNextStartElement()) {
        if (m_xml_.name() == "name") {
            m.name = m_xml_.readElementText();
        } else if (m_xml_.name() == "annotation") {
            while (m_xml_.readNextStartElement()) {
                if (m_xml_.name() == "group") {
                    AnnoItemGroup * g = new AnnoItemGroup;
                    ReadAnnoGroup(*g);
                    if (g->pos.isEmpty()) {
                        delete g;
                    } else {
                        m.groups << g;
                    }
                } else {
                    m_xml_.skipCurrentElement();
                }
            }
        } else if (m_xml_.name() == "preference") {
            ReadPref(m.pref);
        } else {
            m_xml_.skipCurrentElement();
        }
    }
}

//-------------------------------------------------------
void ModalityProperty::ReadPref(ModalityPref & p)
{
    Q_ASSERT(m_xml_.isStartElement() && m_xml_.name() == "preference");
    double f;
    while (m_xml_.readNextStartElement()) {
        if (m_xml_.name() == "adjust") {
            while (m_xml_.readNextStartElement()) {
                if (m_xml_.name() == "factor") {
                    f = m_xml_.readElementText().toDouble();
                    f = f > 0 ? f : 20;
                    p.adjust_factor = f;
                } else {
                    m_xml_.skipCurrentElement();
                }
            }
        } else if (m_xml_.name() == "zoom") {
            while (m_xml_.readNextStartElement()) {
                if (m_xml_.name() == "factor") {
                    f = m_xml_.readElementText().toDouble();
                    f = f > 0 ? f : 0.02;
                    p.zoom_factor = f;
                } else if (m_xml_.name() == "max") {
                    f = m_xml_.readElementText().toDouble();
                    f = f > 0 ? f : 10;
                    p.zoom_max = f;
                } else if (m_xml_.name() == "min") {
                    f = m_xml_.readElementText().toDouble();
                    f = f > 0 ? f : 0.05;
                    p.zoom_min = f;
                } else {
                    m_xml_.skipCurrentElement();
                }
            }
        } else if (m_xml_.name() == "magnifier") {
            while (m_xml_.readNextStartElement()) {
                if (m_xml_.name() == "incremental") {
                    f = m_xml_.readElementText().toDouble();
                    f = f > 0 ? f : 1;
                    p.magnifier_inc = f;
                } else if (m_xml_.name() == "max") {
                    f = m_xml_.readElementText().toDouble();
                    f = f > 0 ? f : 20;
                    p.magnifier_max = f;
                } else if (m_xml_.name() == "min") {
                    f = m_xml_.readElementText().toDouble();
                    f = f > 0 ? f : 2;
                    p.magnifier_min = f;
                } else {
                    m_xml_.skipCurrentElement();
                }
            }
        } else {
            m_xml_.skipCurrentElement();
        }
    }
}

//-------------------------------------------------------
void ModalityProperty::ReadAnnoGroup(AnnoItemGroup & g)
{
    Q_ASSERT(m_xml_.isStartElement() && m_xml_.name() == "group");
    while (m_xml_.readNextStartElement()) {
        if (m_xml_.name() == "position") {
            g.pos = m_xml_.readElementText();
        } else if (m_xml_.name() == "item") {
            AnnoItem * i = new AnnoItem;
            while (m_xml_.readNextStartElement()) {
                if (m_xml_.name() == "text") {
                    i->text = m_xml_.readElementText();
                } else if (m_xml_.name() == "tags") {
                    while (m_xml_.readNextStartElement()) {
                        if (m_xml_.name() == "tag") {
                            DcmTagKey k;
                            while (m_xml_.readNextStartElement()) {
                                if (m_xml_.name() == "group") {
                                    k.setGroup(m_xml_.readElementText().toUShort(nullptr, 0));
                                } else if (m_xml_.name() == "element") {
                                    k.setElement(m_xml_.readElementText().toUShort(nullptr, 0));
                                } else {
                                    m_xml_.skipCurrentElement();
                                }
                            }
                            if (k.hasValidGroup()) {
                                i->keys << k;
                            }
                        } else {
                            m_xml_.skipCurrentElement();
                        }
                    }
                } else {
                    m_xml_.skipCurrentElement();
                }
            }
            if (i->text.isEmpty()) {
                delete i;
            } else {
                g.items << i;
            }
        } else {
            m_xml_.skipCurrentElement();
        }
    }
}
