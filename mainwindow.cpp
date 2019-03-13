#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <chrono>
#include <QSqlError>
#include <QByteArray>
#include <sqlite3.h>
using namespace std;
using namespace BOKZMath;

void MainWindow::saveSettings()
{
    settings->setValue("splitter", ui->splitter_2->saveState());
}

void MainWindow::loadSettings()
{
    ui->splitter_2->restoreState(settings->value("splitter").toByteArray());
}



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), surCat("cat/"),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    settings = new QSettings(QDir::currentPath() + "/" + "config.ini", QSettings::IniFormat, this);
    loadSettings();
    ui->plotToolButton->setIcon(QIcon(":/iconfinder_telescopeastronomyscienceskywatchingobservestars_3441002 (1).png"));
    initStarInfo();
    plotter.setPlot(ui->mainPlot);
    plotter.setAxisXName("Прямое восхождение, градусы");
    plotter.setAxisYName("Склонение, градусы");
    //plotter.setRangeAxisX(0, 360);
    //plotter.setRangeAxisY(-90, 90);
    plotter.setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectItems | QCP::iSelectPlottables| QCP::iSelectLegend);
    plotter.setSelectable(QCP::SelectionType::stNone);
    plotter.setUseLegend(false);
    plotter.setAutoReplot(false);
}

void MainWindow::initSurStarHeaders()
{
    //    surroundStars->setHeaderData(1, Qt::Horizontal, tr("α, град."));
    //    surroundStars->setHeaderData(2, Qt::Horizontal, tr("δ, град."));
    //    surroundStars->setHeaderData(3, Qt::Horizontal, tr("Mv"));
    //    for (qint32 i = 0; i < surroundStars->columnCount(); i++)
    //    {
    //        ui->surrStarTableView->showColumn(i);
    //    }
    //    ui->surrStarTableView->hideColumn(0);

    //    for (qint32 i = 4; i < surroundStars->columnCount(); i++)
    //    {
    //        ui->surrStarTableView->hideColumn(i);
    //    }
    //ui->surrStarTableView->horizontalHeader()->moveSection(3, 0);
}

void MainWindow::initStarInfo()
{
    surroundStarsDist = new QSqlQueryModel(this);
    mainStars = new QSqlTableModel(this, surCat.getDb());
    mainStars->setTable("stars");

    mainStars->setHeaderData(0, Qt::Horizontal, tr("Номер HIP"));
    mainStars->setHeaderData(1, Qt::Horizontal, tr("α, град."));
    mainStars->setHeaderData(2, Qt::Horizontal, tr("δ, град."));
    mainStars->setHeaderData(3, Qt::Horizontal, tr("Mv"));
    mainStars->setHeaderData(4, Qt::Horizontal, tr("Et"));
    mainStars->setHeaderData(5, Qt::Horizontal, tr("Sc"));
    ui->mainStarTableView->setModel(mainStars);
    for (qint32 i = 6; i < mainStars->columnCount(); i++)
    {
        ui->mainStarTableView->hideColumn(i);
    }

    surroundStars = new QSqlTableModel(this, surCat.getDb());
    surroundStars->setTable("star_ocr");
    ui->surrStarTableView->setModel(surroundStars);
    ui->mainStarTableView->horizontalHeader()->moveSection(3, 1);

    initSurStarHeaders();
    QSqlTableModel* comboBoxStarNumModel = new QSqlTableModel(this, surCat.getDb());
    comboBoxStarNumModel->setTable("stars");
    comboBoxStarNumModel->select();
    comboBoxStarNumModel->setSort(0, Qt::SortOrder::AscendingOrder);
    ui->starNumComboBox->setModel(comboBoxStarNumModel);
    ui->starNumComboBox->setModelColumn(0);

}



MainWindow::~MainWindow()
{
    saveSettings();
    delete ui;
}



#include <QSqlRecord>

void MainWindow::fillSurroundStars(QSqlQuery& surStarsQuery)
{
    while (surStarsQuery.next())
    {
        QVector <double> x {surStarsQuery.value("alpha").toDouble()};
        QVector <double> y {surStarsQuery.value("delta").toDouble()};
        plotter.setStyle(QCPGraph::lsNone, QCPScatterStyle::ssStar);
        plotter.setCustomColor(QColor(0x209fdf));
        plotter.setGraphName("");
        plotter.addDefaultGraph(y, x);
        plotter.setNeedRescale(false);
        QCPItemTracer* tracer = plotter.setTracer(ui->mainPlot->graphCount() - 1, x[0], true, QCPItemTracer::tsCircle);
        qint32 id = surStarsQuery.value("id").toInt();
        connect(tracer, &QCPItemTracer::selectionChanged, [this, id, surStarsQuery](bool selected) {
            if (selected)
            {
                QSqlQueryModel* m = qobject_cast <QSqlQueryModel*> (ui->surrStarTableView->model());
                if (m)
                {
                    auto match = m->match(m->index(0, surStarsQuery.record().indexOf("id")), Qt::DisplayRole, id, 1);
                    if (match.isEmpty())
                    {
                        while (m->canFetchMore())
                        {
                            m->fetchMore();
                        }
                        match = m->match(m->index(0, surStarsQuery.record().indexOf("id")), Qt::DisplayRole, id, 1);
                        //Q_ASSERT(!match.isEmpty());
                    }
                    if (!match.isEmpty())
                    {
                        ui->surrStarTableView->scrollTo(match.first().sibling(match.first().row(), 0));
                        ui->surrStarTableView->selectionModel()->setCurrentIndex(match.first().sibling(match.first().row(), 0), QItemSelectionModel::Select | QItemSelectionModel::Rows);

                    }
                }
            }
            else
            {
                ui->surrStarTableView->selectionModel()->clearSelection();
            }
        });
    }
}

void MainWindow::showStarSurround(double l, double m, double n, double surroundRadius, qint32 from, qint32 to)
{
    QString surQueryMainPart = QString("SELECT degrees(acosm((l * %1 + m * %2 + n * %3))) * 60 AS calced, id, alpha, delta, mv FROM star_ocr "
                                       "WHERE %4 ORDER BY calced")
            .arg(l, 0, 'g', 15)
            .arg(m, 0, 'g', 15)
            .arg(n, 0, 'g', 15);

    QSqlQuery surStarsQuery(surCat.getDb());
    QString surFilter = QString("id >= %1 AND id < (%1 + %2) AND (l * %3 + m * %4 + n * %5) >= %6")
            .arg(from)
            .arg(to)
            .arg(l, 0, 'g', 15)
            .arg(m, 0, 'g', 15)
            .arg(n, 0, 'g', 15)
            .arg(cos((surroundRadius / 60) * degreesToRad), 0, 'g', 15);

    surStarsQuery.exec(surQueryMainPart.arg(surFilter));
    surroundStarsDist->setQuery(surStarsQuery);
    ui->surrStarTableView->setModel(surroundStarsDist);

    surroundStarsDist->setHeaderData(0, Qt::Horizontal, tr("Расстояние, мин"));
    surroundStars->setHeaderData(2, Qt::Horizontal, tr("α, град."));
    surroundStars->setHeaderData(3, Qt::Horizontal, tr("δ, град."));
    surroundStars->setHeaderData(4, Qt::Horizontal, tr("Mv"));

    for (qint32 i = 0; i < surroundStarsDist->columnCount(); i++)
    {
        ui->surrStarTableView->showColumn(i);
    }

    ui->surrStarTableView->horizontalHeader()->moveSection(4, 1);
    ui->surrStarTableView->horizontalHeader()->moveSection(2, 4);
    //inited = true;

    surStarsQuery.seek(-1);
    fillSurroundStars(surStarsQuery);
}

void MainWindow::on_plotToolButton_clicked()
{
    try
    {
        plotter.setNeedRescale(true);
        ui->surrStarTableView->setModel(nullptr);
        surroundStars->clear();
        ui->mainPlot->clearPlottables();
        ui->mainPlot->clearItems();

        if (ui->starInSurrGroupBox->isChecked())
        {
            double pointAlpha = ui->alphaSpinBox->value();
            if(pointAlpha < 0 || pointAlpha > 360)
            {
                throw pointAlpha;
            }
            double pointDelta = ui->deltaSpinBox->value();
            if(pointDelta < -90 || pointDelta > 90)
            {
                throw pointDelta;
            }

            double viewAngle = ui->radiusAreaSpinBox->value();
            if(viewAngle <= 0 || viewAngle > 180)
            {
                throw viewAngle;
            }

            plotter.setRangeAxisX(0, 0);
            plotter.setRangeAxisY(0, 0);
            plotter.setAxisXCenter(0);
            plotter.setAxisYCenter(0);

            double lCenter = cos(pointDelta * degreesToRad) * cos(pointAlpha * degreesToRad);
            double mCenter = cos(pointDelta * degreesToRad) * sin(pointAlpha * degreesToRad);
            double nCenter = sin(pointDelta * degreesToRad);

            QString filter = QString(" (l * %1 + m * %2 + n * %3) >= %4")
                    .arg(lCenter)
                    .arg(mCenter)
                    .arg(nCenter)
                    .arg(cos(viewAngle * degreesToRad));
            QSqlQuery mainStarsQuery(surCat.getDb());
            mainStarsQuery.exec(QString("SELECT hip_num, alpha, delta FROM stars WHERE %1")
                                .arg(filter));

            mainStars->setFilter(filter);
            mainStars->select();

            while (mainStarsQuery.next())
            {
                QVector <double> x {mainStarsQuery.value("alpha").toDouble()};
                QVector <double> y {mainStarsQuery.value("delta").toDouble()};
                plotter.setStyle(QCPGraph::lsNone, QCPScatterStyle(QCPScatterStyle::ssDisc, 24));
                plotter.setCustomColor(Qt::red);
                plotter.setGraphName("");
                plotter.addDefaultGraph(y, x);
                QCPItemTracer* tracer = plotter.setTracer(ui->mainPlot->graphCount() - 1, x[0], true, QCPItemTracer::tsCircle);
                qint32 id = mainStarsQuery.value(0).toInt();
                tracer->setSize(24);
                connect(tracer, &QCPItemTracer::selectionChanged, [this, id](bool selected) {
                    if (selected)
                    {
                        auto match = mainStars->match(mainStars->index(0, 0), Qt::DisplayRole, id, 1);
                        if (match.isEmpty())
                        {
                            while (mainStars->canFetchMore())
                            {
                                mainStars->fetchMore();
                            }
                            match = mainStars->match(mainStars->index(0, 0), Qt::DisplayRole, id, 1);
                            Q_ASSERT(!match.isEmpty());
                        }

                        on_mainStarTableView_pressed(match.first());
                        ui->mainStarTableView->scrollTo(match.first());
                        ui->mainStarTableView->selectionModel()->setCurrentIndex(match.first(), QItemSelectionModel::Select|QItemSelectionModel::Rows);
                    }
                    else
                    {
                        ui->mainStarTableView->selectionModel()->clearSelection();
                    }
                });
            }

            QSqlQuery surStarsQuery(surCat.getDb());
            surStarsQuery.exec(QString("SELECT id, alpha, delta FROM star_ocr WHERE %1")
                               .arg(filter));

            fillSurroundStars(surStarsQuery);

            QCPGraph* projectionCenter = new QCPGraph(ui->mainPlot->xAxis, ui->mainPlot->yAxis);
            projectionCenter->setPen(QColor(255, 0, 0, 200));
            projectionCenter->setLineStyle(QCPGraph::lsNone);
            projectionCenter->setScatterStyle(QCPScatterStyle::ssCross);
            plotter.setStyle(QCPGraph::lsNone, QCPScatterStyle::ssCross);
            plotter.setCustomColor(Qt::red);
            plotter.addDefaultGraph(QVector <double> {pointDelta}, QVector <double> {pointAlpha}, false);
            plotter.setGraphName("Центр проектирования");
            initSurStarHeaders();

        }
        else if (ui->starSurrGroupBox->isChecked())
        {
            double surroundRadius = ui->radiusStarSpinBox->value();
            plotter.setRangeAxisX(surroundRadius / 60, surroundRadius / 60);
            plotter.setRangeAxisY(surroundRadius / 60, surroundRadius / 60);

            if(surroundRadius / 60 <= 0 || surroundRadius / 60 > 180)
            {
                throw surroundRadius;
            }
            QSqlQuery mainStarsQuery(surCat.getDb());

            QString mainFilter = QString("hip_num = %1")
                    .arg(ui->starNumComboBox->currentText());
            mainStars->setFilter(mainFilter);
            mainStars->select();

            mainStarsQuery.exec(QString("SELECT alpha, delta, l, m, n, n1, n2 FROM stars WHERE  %1")
                                .arg(mainFilter));
            mainStarsQuery.next();

            QVector <double> x {mainStarsQuery.value("alpha").toDouble()};
            QVector <double> y {mainStarsQuery.value("delta").toDouble()};
            plotter.setAxisXCenter(x.first());
            plotter.setAxisYCenter(y.first());
            plotter.setStyle(QCPGraph::lsNone, QCPScatterStyle(QCPScatterStyle::ssDisc, 24));
            plotter.setCustomColor(QColor(0xf6a625));
            plotter.setGraphName("");
            plotter.addDefaultGraph(y, x);

            qint32 from = mainStarsQuery.value("n2").toInt();
            qint32 to = mainStarsQuery.value("n1").toInt();
            double l = mainStarsQuery.value("l").toDouble();
            double m = mainStarsQuery.value("m").toDouble();
            double n = mainStarsQuery.value("n").toDouble();
            showStarSurround(l, m, n, surroundRadius, from, to);

        }
        ui->mainPlot->replot();
    }
    catch(...)
    {

    }
}

void MainWindow::on_starSurrGroupBox_toggled(bool arg1)
{
    if (arg1)
    {
        ui->starInSurrGroupBox->setChecked(false);
        ui->addFilterGroupBox->setChecked(false);
        ui->starInSurrGroupBox->setEnabled(false);
        ui->addFilterGroupBox->setEnabled(false);
    }
    else
    {
        ui->starInSurrGroupBox->setEnabled(true);
        ui->addFilterGroupBox->setEnabled(true);
    }
}

void MainWindow::on_starInSurrGroupBox_toggled(bool arg1)
{
    if (arg1)
    {
        ui->starSurrGroupBox->setChecked(false);
        ui->starSurrGroupBox->setEnabled(false);
    }
    else
    {
        ui->starSurrGroupBox->setEnabled(true);
    }
}



//void MainWindow::initSurroundStarsDist(double lmnMain[3])
//{
//    //degrees
////    QString filter = QString(" (l * %1 + m * %2 + n * %3) >= %4")
////            .arg(lCenter)
////            .arg(mCenter)
////            .arg(nCenter)
////            .arg(cos(viewAngle * degreesToRad));
//    QSqlQuery mainStarsQuery(surCat.getDb());
//    surroundStarsDist->setQuery(QSqlQuery("SELECT"))
//    mainStarsQuery.exec(QString("SELECT degrees(acos()) * 60, id, alpha, delta FROM star_ocr WHERE %1")
//                        .arg(filter));
//}





void MainWindow::on_mainStarTableView_pressed(const QModelIndex &index)
{
    double l, m, n;
    qint32 from, to;
    ui->surrStarTableView->setModel(nullptr);
    l = index.sibling(index.row(), MainStar::L).data().toDouble();
    m = index.sibling(index.row(), MainStar::M).data().toDouble();
    n = index.sibling(index.row(), MainStar::N).data().toDouble();
    from = index.sibling(index.row(), MainStar::From).data().toInt();
    to = index.sibling(index.row(), MainStar::To).data().toInt();
    showStarSurround(l, m, n , fifteenMinutes, from, to);
}
