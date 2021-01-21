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

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmdata/cmdlnarg.h"
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/ofstd/ofconapp.h"


class myDcmItem: public DcmItem {
  public:
    myDcmItem(const DcmItem &old): DcmItem(old) {
    }
    virtual ~myDcmItem() {}
    DcmList *GetDcmList()const {
        return this->elementList;
    }
};

// qDebug() /*&lt;&lt; "item->getTag : "*/ &lt;< item>getTag().toString().data();
// qDebug() &lt;&lt; " i = " &lt;< i xss=removed>nextInContainer(NULL);
// qDebug() /*&lt;&lt;"item->getTag : "*/&lt;< item>getTag().toString().data();
int main() {
    DcmFileFormat dfile;
    dfile.loadFile("C:/Users/77935/Pictures/dcm/source.dcm");
    myDcmItem *meta_info = new myDcmItem(*dfile.getDataset());
    DcmList *elementList = meta_info->GetDcmList();
    if (!elementList->empty()) {
        DcmObject *dO;
        elementList->seek(ELP_first);
        do {
            dO = elementList->get();
//            std::cout << dO->getTag() << " "
//                      << dO->getTag().getVRName() << " "
////                      << dO->getVRName() << " "
//                      << std::endl;
            dO->print(std::cout, 0, 0, NULL, NULL);
        } while (elementList->seek(ELP_next));
    }
//    std::cout << OFendl;
//    std::cout << "# Dicom-File-Format";
}
