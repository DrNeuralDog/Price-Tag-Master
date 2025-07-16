#include <QApplication>
#include <QDebug>
#include "mainwindow.h"


int main (int argc, char *argv[])
{
    QApplication app (argc, argv);

    app.setApplicationName ("Price Tag Master");
    app.setOrganizationName ("Price Tag Inc.");
    app.setOrganizationDomain ("pricetagmaster.com");


    MainWindow mainWindow;

    mainWindow.setWindowTitle ("Price Tag Master");
    mainWindow.resize (800, 600);
    mainWindow.show ();


    return app.exec ();
} 
