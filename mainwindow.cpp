#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <chrono>
#include <QSqlError>
#include <QByteArray>
#include <sqlite3.h>
#include <gms.h>
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


    //plotter.setRangeAxisX(0, 360);
    //plotter.setRangeAxisY(-90, 90);
    plotter.setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectItems | QCP::iSelectPlottables| QCP::iSelectLegend);
    plotter.setSelectable(QCP::SelectionType::stNone);
    plotter.setUseLegend(false);
    plotter.setAutoReplot(false);
    ui->subPlot->setVisible(false);
    fillSampleComboBox(ui->showSampleComboBox, true);
    fillSampleComboBox(ui->fSampleComboBox, false);
    fillSampleComboBox(ui->sSampleComboBox, false);

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





void MainWindow::fillSurroundStars(QSqlQuery& surStarsQuery)
{

    qint32 count = 0;
    while (surStarsQuery.next())
    {
        ++count;
        QVector <double> x;
        QVector <double> y;
        if (mainStarOffset.isNull())
        {
            x.append(surStarsQuery.value("alpha").toDouble());
            y.append(surStarsQuery.value("delta").toDouble());
        }
        else
        {
            QVector2D tmp ((surStarsQuery.value("alpha").toDouble() - mainStarOffset.x()) * 60,
                           (surStarsQuery.value("delta").toDouble() - mainStarOffset.y()) * 60);
            tmp.normalize();
            tmp *= surStarsQuery.value("calced").toDouble();
            x.append((surStarsQuery.value("alpha").toDouble() - mainStarOffset.x()) * 60);
            y.append((surStarsQuery.value("delta").toDouble() - mainStarOffset.y()) * 60);
            //x.append((surStarsQuery.value("alpha").toDouble()));
            //y.append((surStarsQuery.value("delta").toDouble()));
        }


        plotter.setStyle(QCPGraph::lsNone, QCPScatterStyle::ssStar);
        plotter.setCustomColor(QColor(0x209fdf));
        plotter.setGraphName("");
        plotter.addDefaultGraph(y, x);
        // plotter.setNeedRescale(false);
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
    ui->surrStarGroupBox->setTitle(QString("Звёзды окружения") + " (" + QString::number(count) + ")");
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
        ui->saveCurrentSample->setIcon(QIcon(":/resources/iconfinder_Alert_132559.png"));
        plotter.setNeedRescale(true);
        ui->surrStarTableView->setModel(nullptr);
        surroundStars->clear();
        ui->mainPlot->clearPlottables();
        ui->mainPlot->clearItems();

        if (ui->starInSurrGroupBox->isChecked())
        {
            plotter.setXAxisReversed(true);
            plotter.setAxisXName("Прямое восхождение, градусы");
            plotter.setAxisYName("Склонение, градусы");
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

            mainStarOffset.setX(0);
            mainStarOffset.setY(0);
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
            ui->mainPlot->replot();

        }
        else if (ui->starSurrGroupBox->isChecked())
        {
            plotter.setXAxisReversed(true);
            plotter.setAxisXName("Прямое восхождение, минуты");
            plotter.setAxisYName("Склонение, минуты");

            double surroundRadius = ui->radiusStarSpinBox->value();
            if (surroundRadius <= 0 || surroundRadius  > 180)
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

            mainStarOffset.setX(mainStarsQuery.value("alpha").toDouble());
            mainStarOffset.setY(mainStarsQuery.value("delta").toDouble());
            double cosCoef = cos(mainStarOffset.y() * degreesToRad);
            plotter.setRangeAxisX(fifteenMinutes / cosCoef, fifteenMinutes / cosCoef);
            plotter.setRangeAxisY(fifteenMinutes, fifteenMinutes);
            if (!qFuzzyCompare(ui->ellipseSpinBox->value(), 0.))
            {
                double v = ui->ellipseSpinBox->value();
                QCPItemEllipse* ellipse = new QCPItemEllipse(ui->mainPlot);
                ellipse->topLeft->setCoords(v / cosCoef, v);
                ellipse->bottomRight->setCoords(- v / cosCoef, -v);
            }


            //            QVector <double> x {mainStarsQuery.value("alpha").toDouble()};
            //            QVector <double> y {mainStarsQuery.value("delta").toDouble()};
            //            plotter.setAxisXCenter(x.first());
            //            plotter.setAxisYCenter(y.first());

            //            QVector <double> x {0};
            //            QVector <double> y {0};
            //            plotter.setAxisXCenter(x.first());
            //            plotter.setAxisYCenter(y.first());
            //            mainStarOffset.setX(mainStarsQuery.value("alpha").toDouble());
            //            mainStarOffset.setY(mainStarsQuery.value("delta").toDouble());

            QVector <double> x {0};
            QVector <double> y {0};
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
            ui->mainPlot->replot();
        }
        else if (ui->sampleGroupBox->isChecked())
        {
            makeSample();
        }

    }
    catch(...)
    {
        QMessageBox::warning(this, "Ошибка", "Что-то пошло не так!"); //tmp
    }
}

void MainWindow::on_starSurrGroupBox_toggled(bool arg1)
{
    if (arg1)
    {
        ui->starInSurrGroupBox->setChecked(false);
        ui->sampleGroupBox->setChecked(false);
        ui->starInSurrGroupBox->setEnabled(false);
        ui->sampleGroupBox->setEnabled(false);
    }
    else
    {
        ui->starInSurrGroupBox->setEnabled(true);
        ui->sampleGroupBox->setEnabled(true);
    }
}

void MainWindow::on_starInSurrGroupBox_toggled(bool arg1)
{
    if (arg1)
    {
        ui->starSurrGroupBox->setChecked(false);
        ui->starSurrGroupBox->setEnabled(false);
        ui->sampleGroupBox->setChecked(false);
        ui->sampleGroupBox->setEnabled(false);
    }
    else
    {
        ui->starSurrGroupBox->setEnabled(true);
        ui->sampleGroupBox->setEnabled(true);
    }
}

void MainWindow::on_sampleGroupBox_toggled(bool arg1)
{
    if (arg1)
    {
        ui->starSurrGroupBox->setChecked(false);
        ui->starSurrGroupBox->setEnabled(false);
        ui->starInSurrGroupBox->setChecked(false);
        ui->starInSurrGroupBox->setEnabled(false);
    }
    else
    {
        ui->starInSurrGroupBox->setEnabled(true);
        ui->starSurrGroupBox->setEnabled(true);
    }
}



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

void MainWindow::on_chooseSaveFile_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                    "",
                                                    tr("txt files (*.txt)"));
    currentOutputFile.setFileName(fileName);
    if (!currentOutputFile.open(QIODevice::Append))
    {
        QMessageBox::warning(this, "Ошибка", "Не удалось открыть файл!");
        return;
    }
    ui->chooseSaveFile->setIcon(QIcon(":/resources/iconfinder_Yes_132508.png"));
}

void MainWindow::on_saveCurrentSample_clicked()
{
    QString comment = QInputDialog::getText(this, "Комментарий к выборке", "", QLineEdit::Normal, "Без комментариев");
    if (currentOutputFile.isOpen())
    {
        QTextStream out(&currentOutputFile);
        if (currentOutputFile.size() == 0)
        {
            QString header;
            for (qint32 i = 0; i < mainStars->columnCount(); ++i)
            {
                header.append(mainStars->headerData(i, Qt::Horizontal).toString() + "\t");
            }
            header.chop(1);
            header.append("\r\n");
            out << header;
        }
        for (qint32 i = 0; i < mainStars->rowCount(); ++i)
        {
            QString line;
            for (qint32 j = 0; j < mainStars->columnCount(); j++)
            {
                line.append(mainStars->data(mainStars->index(i, j)).toString() + "\t");
            }
            line.append(comment);
            line.append("\r\n");
            out << line;
        }
        ui->saveCurrentSample->setIcon(QIcon(":/resources/iconfinder_Yes_132508.png"));
    }
    else
    {
        QMessageBox::warning(this, "Ошибка", "Не выбран файл для сохранения!");
    }
}

void MainWindow::on_mainStarTableView_doubleClicked(const QModelIndex &index)
{
    double alpha = index.sibling(index.row(), MainStar::Alpha).data().toDouble();
    double delta = index.sibling(index.row(), MainStar::Delta).data().toDouble();
    ui->alphaSpinBox->setValue(alpha);
    ui->deltaSpinBox->setValue(delta);
}



void MainWindow::plotSampleHistogramms()
{
    plotter.setAxisXName("Интегральная яркость");
    plotter.setAxisYName("Количество");
    plotter.setXAxisReversed(false);
    plotter.setTitle(QString("Распределение выборки по интегральной яркости (%1 звёзд)")
                     .arg(sample.starNum.size()));
    plotter.addHistogramm(sample.bright, 1000, true);

    plotter.setPlot(ui->subPlot);
    plotter.setTitle(QString("Распределение количества соседей"));
    plotter.setAxisXName("Число соседей");
    plotter.setAxisYName("Количество");
    plotter.addHistogramm(sample.neibCount, 1, false);
    plotter.setPlot(ui->mainPlot);
}

void MainWindow::showSampleInTable()
{
    QString filter;
    for (const auto& i : sample.starNum)
    {
        filter.append(QString::number(i) + ",");
    }
    filter.chop(1);
    filter.append(")");
    filter.prepend("hip_num IN (");
    mainStars->setFilter(filter);
    mainStars->select();
}

void MainWindow::makeSample()
{   
    auto model = static_cast <QSqlTableModel*> (ui->starNumComboBox->model());
    sample.clear();
    QSqlQuery checkStarQuery(surCat.getDb());
    checkStarQuery.prepare("SELECT COUNT(*) FROM star_ocr WHERE id >= ? AND id < (? + ?)"
                           "AND (l * ? + m * ? + n * ?) BETWEEN ? AND ?");
    for (qint32 i = 0; i < model->rowCount(); ++i)
    {
        double bright = model->data(model->index(i, MainStar::Bright)).toDouble();
        if (ui->brighnessCheckBox->isChecked())
        {
            double fromBright = ui->minBrightDoubleSpinBox->value();
            double toBright = ui->maxBrightDoubleSpinBox->value();
            if (!(bright >= fromBright
                  && bright <= toBright))
            {
                continue;
            }
        }
        qint32 starNum = model->data(model->index(i, MainStar::HipNum)).toInt();
        qint32 from = model->data(model->index(i, MainStar::From)).toInt();
        qint32 to = model->data(model->index(i, MainStar::To)).toInt();
        double l = model->data(model->index(i, MainStar::L)).toDouble();
        double m = model->data(model->index(i, MainStar::M)).toDouble();
        double n = model->data(model->index(i, MainStar::N)).toDouble();
        double fromDistance = ui->minAreaDoubleSpinBox->value();
        double toDistance = ui->maxAreaDoubleSpinBox->value();
        checkStarQuery.addBindValue(from);
        checkStarQuery.addBindValue(from);
        checkStarQuery.addBindValue(to);
        checkStarQuery.addBindValue(l);
        checkStarQuery.addBindValue(m);
        checkStarQuery.addBindValue(n);
        checkStarQuery.addBindValue(cos((toDistance / 60) * degreesToRad));
        checkStarQuery.addBindValue(cos((fromDistance / 60) * degreesToRad));
        checkStarQuery.exec();
        // qDebug() << checkStarQuery.lastError().text();


        //        checkStarQuery.exec(QString("SELECT COUNT(*) FROM star_ocr WHERE id >= %1 AND id < (%1 + %2)"
        //                                    "AND (l * %3 + m * %4 + n * %5) BETWEEN %6 AND %7")
        //                            .arg(from)
        //                            .arg(to)
        //                            .arg(l, 0, 'g', 15)
        //                            .arg(m, 0, 'g', 15)
        //                            .arg(n, 0, 'g', 15)
        //                            .arg(cos((toDistance / 60) * degreesToRad))
        //                            .arg(cos((fromDistance / 60) * degreesToRad)));
        // qDebug() << QString("SELECT COUNT(*) FROM star_ocr WHERE id >= %1 AND id < (%1 + %2)"
        //                     "AND (l * %3 + m * %4 + n * %5) BETWEEN %6 AND %7")
        //.arg(from)
        //    .arg(to)
        // .arg(l, 0, 'g', 15)
        // .arg(m, 0, 'g', 15)
        // .arg(n, 0, 'g', 15)
        // .arg(cos((fromDistance / 60) * degreesToRad))
        // .arg(cos((toDistance / 60) * degreesToRad));
        checkStarQuery.next();
        qint32 starCount = checkStarQuery.value(0).toInt();
        if (starCount >= ui->minStarSurSpinBox->value()
                && starCount <= ui->maxStarSurSpinBox->value())
        {
            sample.starNum.append(starNum);
            sample.neibCount.append(starCount);
            sample.bright.append(bright);
        }

    }
    if (sample.starNum.size() > 0)
    {
        sample.isActive = true;
    }
    plotSampleHistogramms();
    showSampleInTable();

}

void MainWindow::fillSampleComboBox(QComboBox* comboBox, bool update)
{
    QSqlQuery sampleListQuery(surCat.getDb());
    sampleListQuery.exec("SELECT  DISTINCT sample_desc FROM samples;");
    static QStringList list;
    if (update)
    {
        list.clear();
        while (sampleListQuery.next())
        {
            list.append(sampleListQuery.value(0).toString());
        }
    }
    comboBox->clear();
    comboBox->addItems(list);
}

void MainWindow::on_saveSamplePushButton_clicked()
{
    if (sample.isActive && !ui->sampleNameLineEdit->text().isEmpty())
    {
        QSqlQuery lastSampleIdQuery(surCat.getDb());
        lastSampleIdQuery.exec("SELECT MAX(sample_id) FROM samples");
        lastSampleIdQuery.next();
        qint32 sampleId = lastSampleIdQuery.value(0).toInt() + 1;
        QSqlQuery insertSampleQuery(surCat.getDb());
        insertSampleQuery.prepare("INSERT INTO samples (sample_id, hip_num, sample_desc, neib_count) "
                                  "VALUES (?, ?, ?, ?)");
        for (qint32 i = 0; i < sample.starNum.size(); ++i)
        {
            insertSampleQuery.addBindValue(sampleId);
            insertSampleQuery.addBindValue(sample.starNum[i]);
            insertSampleQuery.addBindValue(ui->sampleNameLineEdit->text());
            insertSampleQuery.addBindValue(sample.neibCount[i]);
            insertSampleQuery.exec();
            // qDebug() << insertSampleQuery.lastError().text();
        }
    }
    fillSampleComboBox(ui->showSampleComboBox, true);
    fillSampleComboBox(ui->fSampleComboBox, false);
    fillSampleComboBox(ui->sSampleComboBox, false);
}



void MainWindow::on_joinSamplePushButton_clicked()
{
    if (!ui->joinSampleName->text().isEmpty())
    {
        QSqlQuery sampleSelect(surCat.getDb());
        sampleSelect.exec(QString("UPDATE samples SET sample_desc = '%1' WHERE sample_desc = '%2' OR sample_desc = '%3'")
                          .arg(ui->joinSampleName->text())
                          .arg(ui->fSampleComboBox->currentText())
                          .arg(ui->sSampleComboBox->currentText()));
        fillSampleComboBox(ui->showSampleComboBox, true);
        fillSampleComboBox(ui->fSampleComboBox, false);
        fillSampleComboBox(ui->sSampleComboBox, false);
    }


}

void MainWindow::on_showPushButton_clicked()
{
    sample.clear();
    QSqlQuery sampleSelect(surCat.getDb());
    sampleSelect.exec(QString("SELECT sa.hip_num, neib_count, integral_bright "
                              "FROM samples AS sa "
                              "INNER JOIN stars AS st ON (st.hip_num = sa.hip_num) WHERE sample_desc = '%1'")
                      .arg(ui->showSampleComboBox->currentText()));
    sample.isActive = true;
    while (sampleSelect.next())
    {
        sample.starNum.append(sampleSelect.value("hip_num").toDouble());
        sample.neibCount.append(sampleSelect.value("neib_count").toDouble());
        sample.bright.append(sampleSelect.value("integral_bright").toDouble());
    }
    plotSampleHistogramms();
    showSampleInTable();

}

void MainWindow::on_unloadTxtPushButton_clicked()
{
    QSqlQuery sampleSelect(surCat.getDb());
    sampleSelect.exec(QString("SELECT sa.hip_num, neib_count, integral_bright "
                              "FROM samples AS sa "
                              "INNER JOIN stars AS st ON (st.hip_num = sa.hip_num) WHERE sample_desc = '%1'")
                      .arg(ui->showSampleComboBox->currentText()));
    sample.isActive = true;
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                                                    "",
                                                    tr("Images (*.txt)"));
    QFile file (fileName);
    if (file.open(QIODevice::WriteOnly))
    {
        QTextStream out (&file);
        out << "num" << "\t" << "neib_coung" << "\t" << "bright" << endl;
        while (sampleSelect.next())
        {
            out << sampleSelect.value("hip_num").toString() << "\t" << sampleSelect.value("neib_count").toString() << "\t" << sampleSelect.value("integral_bright").toString() << endl;
        }
    }

}

void MainWindow::on_unloadBinPushButton_clicked()
{

}

void MainWindow::on_loadHandledSamplePushButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                    "",
                                                    tr("txt files (*.txt)"));
    bool error = false;
    if (!fileName.isEmpty() && !ui->handledSampleNameLineEdit->text().isEmpty())
    {
        QFile f (fileName);
        if (f.open(QIODevice::ReadOnly))
        {
            QSqlQuery insertSampleName(surCat.getDb());
            surCat.getDb().transaction();
            insertSampleName.prepare("INSERT INTO handled_sample_name (name) VALUES (?)");
            insertSampleName.addBindValue(ui->handledSampleNameLineEdit->text());
            if (error = (insertSampleName.exec() &&
                         insertSampleName.exec("SELECT MAX(id) FROM handled_sample_name")))
            {
                insertSampleName.next();
                qint32 sampleId = insertSampleName.value(0).toInt();
                QTextStream in (&f);
                QString line;
                QSqlQuery insertHandled(surCat.getDb());
                insertHandled.prepare("INSERT INTO handled_samples (sample_id, hip_num, alpha_cor, "
                                      "delta_cor, u_alpha, u_delta, bright, pixel_count, weight, flag_bright, "
                                      "u_alpha_rand, u_delta_rand)"
                                      "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ? , ?)");
                while (in.readLineInto(&line))
                {
                    line.remove("”");
                    auto list = line.split(" ", QString::SkipEmptyParts);
                    auto alpha = list[1].split("°");
                    double degreeAlpha = alpha[0].toInt();
                    alpha[1].remove("”");
                    alpha = alpha[1].split("’");
                    GMS transformAlpha = GMS(degreeAlpha, alpha[0].toInt(), alpha[1].toDouble());

                    auto delta = list[2].split("°");
                    double degreeDelta = delta[0].toInt();
                    delta[1].remove("”");
                    delta = delta[1].split("’");
                    GMS transformDelta = GMS(degreeDelta, delta[0].toInt(), delta[1].toDouble());
                    insertHandled.addBindValue(sampleId);
                    insertHandled.addBindValue(list[0].toInt());
                    insertHandled.addBindValue(transformAlpha.transToGrad());
                    insertHandled.addBindValue(transformDelta.transToGrad());
                    insertHandled.addBindValue(list[3].toDouble());
                    insertHandled.addBindValue(list[4].toDouble());
                    insertHandled.addBindValue(list[7].toInt());
                    insertHandled.addBindValue(list[8].toInt());
                    insertHandled.addBindValue(list[9].toDouble());
                    insertHandled.addBindValue(list[10].toInt());
                    insertHandled.addBindValue(list[5].toDouble());
                    insertHandled.addBindValue(list[6].toDouble());
                    if (error = !insertHandled.exec())
                    {
                        qDebug() << insertHandled.lastError().text();
                        break;
                    }

                }
            }
            if (error)
            {
                surCat.getDb().rollback();
            }
            else
            {
                surCat.getDb().commit();
            }
        }
    }

}
