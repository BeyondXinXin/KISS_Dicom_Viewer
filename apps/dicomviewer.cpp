#include <global/KissGlobal>
#include <engine/KissEngine>
#include <window/KissWindow>

////----------------------------------------------------------------
//int main1(int argc, char *argv[]) {
//    QApplication a(argc, argv);
//    a.setOrganizationName(Project_OrganizationName);
//    a.setOrganizationDomain(Project_OrganizationDomain);
//    a.setApplicationName(Project_NAME);
//    a.setQuitOnLastWindowClosed(true);
//    using namespace Kiss;
//    QUIHelper::SetStyle(":/Style/style.qss");
////    QUIHelper::SetCode();
////    InitialDir();
////    InitialDao();
////    DicomViewer w(DicomViewer::SingleInstance);
////    w.show();
//    QTreeWidgetItem item;
//    item.setText(0, "aa");
//    QTreeWidgetItem item1;
//    item1.setText(0, "aa");
//    item.addChild(&item1);
//    //
//    return a.exec();
//}

#include "dcmtk/dcmdata/dctk.h"

template<typename T>
class NewDcmItem: public T {
  public:
    NewDcmItem(const T &old): T(old) {
    }
    DcmList *GetDcmList()const {
        return this->elementList;
    }
  protected:
    virtual ~NewDcmItem() {}
};
using MyDcmDataset = NewDcmItem<DcmDataset>;
using MyDcmMetaInfo = NewDcmItem<DcmMetaInfo>;
using MywDcmItem = NewDcmItem<DcmItem>;

//
template<typename T>
void GenerateItems(QList<QTreeWidgetItem *> &items, T &t) {
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
            tmp_item->setText(5, QString::fromLocal8Bit(value.c_str()));
            if(EVR_SQ == dO->getVR()) {
                QList<QTreeWidgetItem *> tmp_items;
                DcmItem *sq;
                t.findAndGetSequenceItem(dO->getTag(), sq);
                MywDcmItem *dcmitem_info = new MywDcmItem(*sq);
                GenerateItems(tmp_items, *dcmitem_info);
                tmp_item->addChildren(tmp_items);
            }
            items << tmp_item;
        } while (elementList->seek(ELP_next));
        delete dO;
    }
}

QTreeWidget *GetWid() {
    QTreeWidget *wid = new QTreeWidget;
    wid->setHeaderLabels(QStringList()
                         << "Tag ID" << "VR" << "VM" << "Length" << "Description" << "value");
    wid->setColumnWidth(0, 200);
    wid->setColumnWidth(1, 70);
    wid->setColumnWidth(2, 100);
    wid->setColumnWidth(3, 50);
    wid->setColumnWidth(4, 300);
    wid->setColumnWidth(5, 300);
    wid->setGeometry(0, 0, 1200, 800);
    return wid;
}

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    using namespace Kiss;
    QUIHelper::SetStyle(":/Style/style.qss");
    // 初始化 TreeWidget
    QTreeWidget *wid = GetWid();
    QList<QTreeWidgetItem *> items;
    // 读取文件
    DcmFileFormat dfile;
    dfile.loadFile("C:/Users/77935/Pictures/dcm/source.dcm");
    //
    QTreeWidgetItem *metainfo_item = new QTreeWidgetItem;
    metainfo_item->setText(0, "Dicom-MetaInfo");
    items << metainfo_item;
    MyDcmMetaInfo *meta_info = new MyDcmMetaInfo(*dfile.getMetaInfo());
    GenerateItems(items, *meta_info);
    QTreeWidgetItem *dataset_item = new QTreeWidgetItem;
    dataset_item->setText(0, "Dicom-Data-Set");
    items << dataset_item;
    MyDcmDataset *dataset_info = new MyDcmDataset(*dfile.getDataset());
    GenerateItems(items, *dataset_info);
    // 显示
    wid->addTopLevelItems(items);
    wid->show();
    return a.exec();
}
