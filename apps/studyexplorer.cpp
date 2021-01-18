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
    QString path = QDir::currentPath();
    QApplication::addLibraryPath(path + QString("/plugins"));
    QPluginLoader loader(path + QString("/plugins/sqldrivers/libqsqlite.so"));
    using namespace Kiss;
    QUIHelper::SetStyle();
    QUIHelper::SetCode();
    InitialDir();
    InitialDao();
    InitialStudyDao();
    StudyExplorerWidget w;
    w.setWindowTitle(Project_NAME);
    w.show();
    return a.exec();
}
