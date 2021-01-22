//#include <global/KissGlobal>
//#include <engine/KissEngine>
//#include <window/KissWindow>

////----------------------------------------------------------------
//int main(int argc, char *argv[]) {
//    QApplication a(argc, argv);
//    a.setOrganizationName(Project_OrganizationName);
//    a.setOrganizationDomain(Project_OrganizationDomain);
//    a.setApplicationName(Project_NAME);
//    a.setQuitOnLastWindowClosed(true);
//    using namespace Kiss;
//    QUIHelper::SetStyle(":/Style/style.qss");
//    QUIHelper::SetCode();
//    InitialDir();
//    InitialDao();
//    DicomViewer w(DicomViewer::SingleInstance);
//    w.show();
//    return a.exec();
//}


#include "dcmtk/dcmdata/dctk.h"

#define Print std::cout<<
#define Printend <<std::endl;
#define PrintemspValue(emsp,value1,value2,value3,value4,value5,value6) \
    for(int i = 0; i < emsp; i++) {Print "\40\40";}\
    Print value1 <<" "  <<value2<<" ["<< value3 <<"] "\
                 << value4 <<" " <<value5 <<" (" <<value6<<")" Printend

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

template<typename T>
void PaintTags(T &t, const int &emsp = 0) {
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
            PrintemspValue(emsp, tag, tag.getVRName(),
                           value, dO->getLength(), dO->getVM(), tag.getTagName())
            if(EVR_SQ == dO->getVR()) {
                DcmItem *sq;
                t.findAndGetSequenceItem(dO->getTag(), sq);
                MywDcmItem *dcmitem_info = new MywDcmItem(*sq);
                PaintTags(*dcmitem_info, emsp + 1);
            }
        } while (elementList->seek(ELP_next));
        delete dO;
    }
}

int main() {
    DcmFileFormat dfile;
    dfile.loadFile("/home/arteryflow/图片/DicomData/DSA/Liyunlong/IMG-0002-00001.dcm");
    Print "DcmMetaInfo------" Printend
    MyDcmMetaInfo *meta_info = new MyDcmMetaInfo(*dfile.getMetaInfo());
    PaintTags(*meta_info);
    Print "DcmDataset------" Printend
    MyDcmDataset *dataset_info = new MyDcmDataset(*dfile.getDataset());
    PaintTags(*dataset_info);
}
