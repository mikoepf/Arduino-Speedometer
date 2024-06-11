#include <QApplication>

#include "mainwindow.h"
#include "connection.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    if(!createconnection()) // Checks whether a connection with Profiles.db could be established.
                            // Also invokes the connection to the Profiles.db.
        return EXIT_FAILURE;
    MainWindow w;
    w.show();
    return a.exec();
}
