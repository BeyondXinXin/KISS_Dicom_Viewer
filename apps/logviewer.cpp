#include "global/KissDicomViewConfig.h"
#include "global/global.h"
#include "global/stable.h"
#include <window/KissWindow>

//----------------------------------------------------------------
int main(int argc, char * argv[])
{
    QApplication a(argc, argv);
    a.setOrganizationName(Project_OrganizationName);
    a.setOrganizationDomain(Project_OrganizationDomain);
    a.setApplicationName(Project_NAME);
    a.setQuitOnLastWindowClosed(true);
    using namespace Kiss;
    QUIHelper::SetStyle();
    QUIHelper::SetCode();
    InitialDir();
    InitialDao();
    InitialLogDao();
    LogDbWidget w;
    w.show();
    return a.exec();
}
