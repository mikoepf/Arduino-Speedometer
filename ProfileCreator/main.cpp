#include <QApplication>

#include "mainwindow.h"
#include "connection.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    if(!createconnection())
        return EXIT_FAILURE;
    MainWindow w;
    w.show();
    return a.exec();
}
