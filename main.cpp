#include <QApplication>
#include <QIcon>
#include <mainwindow.h>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("notNV");
    app.setOrganizationName("yafp");
    app.setOrganizationDomain("notnv.yafp.de");

    MainWindow window;
    window.setWindowIcon(QIcon(":/images/icon_notNV_big.png"));
    window.setWindowTitle("notNV");
    window.show();
    return app.exec();
}
