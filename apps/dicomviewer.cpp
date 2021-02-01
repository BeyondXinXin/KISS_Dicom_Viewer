#include <global/KissGlobal>
#include <engine/KissEngine>
#include <window/KissWindow>

//----------------------------------------------------------------
int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    a.setOrganizationName(Project_OrganizationName);
    a.setOrganizationDomain(Project_OrganizationDomain);
    a.setApplicationName(Project_NAME);
    a.setQuitOnLastWindowClosed(true);
    using namespace Kiss;
    QUIHelper::SetStyle(":/Style/style.qss");
    QUIHelper::SetCode();
    InitialDir();
    InitialDao();
    DicomViewer w(DicomViewer::SingleInstance);
    w.show();
    return a.exec();
}
