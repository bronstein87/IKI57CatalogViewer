#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include "customplot/qcustomplot.h"
#include <QApplication>
#include <QMessageBox>
#include <QDebug>
#include <fstream>
#include <vector>
#include <cmath>
#include <QVector>
#include <QString>
#include <algorithm>
#include <QFileDialog>
#include <QTextStream>
#include <QFile>
#include <QIODevice>
#include <QFileInfo>
#include <QDataStream>
#include <stdio.h>
#include <QtConcurrent/QtConcurrent>
#include <QPair>
#include <mathfunc.h>
#include <QSqlTableModel>
#include <surroundcatalog.h>
#include <customplot/cxyplotter.h>
#include <QModelIndex>
#include <QSettings>

namespace MainStar
{
    enum
    {
        HipNum,
        Alpha,
        Delta,
        Mv,
        Bv,
        Sp,
        L,
        M,
        N,
        To,
        From
    };
};

namespace SurroundStar
{
    enum
    {
        Id,
        Alpha,
        Delta,
        L,
        M,
        N
    };
};


namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void initStarInfo();


    void initSurStarHeaders();

private slots:

    void on_starSurrGroupBox_toggled(bool arg1);

    void on_starInSurrGroupBox_toggled(bool arg1);

    void on_plotToolButton_clicked();


    void on_mainStarTableView_pressed(const QModelIndex &index);

private:

    void fillSurroundStars(QSqlQuery& surStarsQuery);

    void saveSettings();

    void loadSettings();

    void initSurroundStarsDist(qint32 mainStarNum);

    SurroundCatalog surCat;
    QSqlTableModel* surroundStars;
    QSqlTableModel* mainStars;
    QSqlQueryModel* surroundStarsDist;
    CXYPlotter plotter;
    QSettings* settings = nullptr;
    constexpr const static double fifteenMinutes = 15;


    Ui::MainWindow *ui;


    void showStarSurround(double l, double m, double n, double surroundRadius, qint32 from, qint32 to);
};





#endif // MAINWINDOW_H


