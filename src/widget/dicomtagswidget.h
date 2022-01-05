#ifndef DICOMTAGSWIDGET_H
#define DICOMTAGSWIDGET_H

#include "dcmtk/dcmdata/dctk.h"
#include "global/global.h"
#include "ui_dicomtagswidget.h"
#include <QTreeWidget>

template<typename T>
class NewDcmItem : public T
{
public:
    NewDcmItem(const T &old)
      : T(old)
    {
    }
    DcmList *GetDcmList() const
    {
        return this->elementList;
    }

protected:
    virtual ~NewDcmItem()
    {
    }
};
using MyDcmDataset = NewDcmItem<DcmDataset>;
using MyDcmMetaInfo = NewDcmItem<DcmMetaInfo>;
using MywDcmItem = NewDcmItem<DcmItem>;
//

template<typename T>
void GenerateItems(QList<QTreeWidgetItem *> &items, T &t, const QString &t_strCharacterSet = "")
{
    //
    DcmList *elementList = t.GetDcmList();
    if (!elementList->empty()) {
        DcmObject *dO;
        DcmTag tag;
        OFString value;
        elementList->seek(ELP_first);
        do {
            dO = elementList->get();
            tag = dO->getTag();
            DcmElement *elem;
            t.findAndGetElement(tag, elem);
            elem->getOFString(value, 0);
            QTreeWidgetItem *tmp_item = new QTreeWidgetItem;
            tmp_item->setText(0, QString::fromLocal8Bit(tag.toString().c_str()));
            tmp_item->setText(1, tag.getVRName());
            tmp_item->setText(2, QString::number(dO->getLength()));
            tmp_item->setText(3, QString::number(dO->getVM()));
            tmp_item->setText(4, tag.getTagName());
            if (t_strCharacterSet.isEmpty() || t_strCharacterSet.contains("GB")) {
                //                tmp_item->setText(
                //                  5, QString::fromLocal8Bit(Kiss::QUIHelper::String2UTF8(value).c_str()));
            } else {
                tmp_item->setText(
                  5, QString::fromLocal8Bit(value.c_str()));
            }
            if (EVR_SQ == dO->getVR()) {
                QList<QTreeWidgetItem *> tmp_items;
                DcmItem *sq;
                t.findAndGetSequenceItem(dO->getTag(), sq);
                MywDcmItem *dcmitem_info = new MywDcmItem(*sq);
                GenerateItems(tmp_items, *dcmitem_info, t_strCharacterSet);
                tmp_item->addChildren(tmp_items);
            }
            items << tmp_item;
        } while (elementList->seek(ELP_next));
        delete dO;
    }
}

class KissQTreeWidget : public QTreeWidget
{
    Q_OBJECT
public:
    explicit KissQTreeWidget(QWidget *parent = nullptr);
    virtual ~KissQTreeWidget();
Q_SIGNALS:
    void SgnOpenFolder();

protected:
    void Initial();
    virtual void contextMenuEvent(QContextMenuEvent *e);
    QMenu *context_menu_;
    QItemSelection selection_;
};

/**
 * @brief The DicomTagsWidget class
 * 用来显示 DIOCM Tags
 */
class DicomTagsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DicomTagsWidget(const QString &str, QWidget *parent = nullptr);

protected:
    void changeEvent(QEvent *e);

private:
    void SlotFilterChanged();
    void SLot_OpenFolder();

private:
    Ui::DicomTagsWidget ui;
    KissQTreeWidget *tree_wid_;
};

#endif // DICOMTAGSWIDGET_H
