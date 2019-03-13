#include "mainwindow.h"
#include <QApplication>
#include <QFile>
#include <QMessageBox>
#include <QDebug>
#include <QDataStream>
#include <fstream>
#include <iostream>
#include <vector>
#include <cmath>



int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    QStringList paths = QCoreApplication::libraryPaths();
    paths.append(".");
    paths.append("imageformats");
    paths.append("platforms");
    paths.append("sqldrivers");
    MainWindow w;
    w.show();


    return a.exec();
}
