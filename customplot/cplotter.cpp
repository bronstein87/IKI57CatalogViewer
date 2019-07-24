#include "cplotter.h"



CPlotter::CPlotter(QCustomPlot* _plot, QObject* parent): QObject(parent), plot(_plot)
{

}


void CPlotter::addEmptyGraphs(const QStringList &names, qint32 count)
{
    if (!plotIsSetted())
        return;

    if (plotIsEmpty())
    {
        setPlotOptions();
        setPlotterRescale();
    }

    for (qint32 i = 0 ; i < count; i ++ )
    {
        plot->addGraph()->setName(names[i]);
    }
}



void CPlotter::setPlotterRescale()
{
    QPair <double, double> captAxisXRange = axisXRange;
    QPair <double, double> captAxisYRange = axisYRange;
    auto cplot = plot;
    connect(plot, &QCustomPlot::mousePress, [=](auto e) {

        if(e->modifiers() == Qt::NoModifier)
        {
            if (e->buttons() == Qt::RightButton)
            {
                cplot->rescaleAxes();

                if (! (qFuzzyCompare(captAxisXRange.first, 0) && qFuzzyCompare(captAxisXRange.second, 0)))
                {
                    cplot->xAxis->setRange(QCPRange(captAxisXRange.first, captAxisXRange.second));
                }
                if (!(qFuzzyCompare(captAxisYRange.first, 0) && qFuzzyCompare(captAxisYRange.second, 0)))
                {
                    cplot->yAxis->setRange(QCPRange(captAxisYRange.first, captAxisYRange.second));
                }
                cplot->replot();

            }}});
}

void CPlotter::setRectangleZoomComb()
{
    auto cplot = plot;
    auto interact = interactions;
    connect(plot, &QCustomPlot::mousePress, [cplot, interact](auto e) mutable {

        if (e->buttons() == Qt::LeftButton && e->modifiers() == Qt::ControlModifier)
        {
            cplot->setInteractions(QCP::iRangeZoom);
            cplot->setSelectionRectMode(QCP::SelectionRectMode::srmZoom);

        }
        else if (e->buttons() == Qt::LeftButton && e->modifiers() == Qt::NoModifier)
        {
            cplot->setSelectionRectMode(QCP::SelectionRectMode::srmNone);
            cplot->setInteractions(interact);


        }});
}


void CPlotter::setSaveMenu()
{
    auto cplot = plot;
    connect(plot, &QCustomPlot::mousePress, [=](auto e) {
        if (e->buttons() == Qt::RightButton && e->modifiers() == Qt::AltModifier)
        {
            QMenu* menu = new QMenu();
            menu->setAttribute(Qt::WA_DeleteOnClose);
            QAction* savePlotPDF = new QAction(tr("Cохранить в PDF"), menu);
            connect(savePlotPDF, &QAction::triggered, [cplot](){
                QString filename =  QFileDialog::getSaveFileName(nullptr, tr("Save File"),
                                                                 "",
                                                                 tr("Images (*.pdf)"));
                cplot->savePdf(filename);
            });
            QAction* savePlotPNG = new QAction(tr("Cохранить в PNG"), menu);
            connect(savePlotPNG, &QAction::triggered, [cplot](){
                QString filename =  QFileDialog::getSaveFileName(nullptr, tr("Save File"),
                                                                 "",
                                                                 tr("Images (*.png)"));
                cplot->savePng(filename, 600, 600);
            });
            connect(menu, &QMenu::triggered, menu, &QMenu::deleteLater);
            menu->addAction(savePlotPDF);
            menu->addAction(savePlotPNG);
            menu->popup(cplot->mapToGlobal(e->pos()));
        }
    });
}

void CPlotter::setMarkerHideFunction()
{
    int lastItem = plot->legend->itemCount() - 1;
    auto p = plot;
    connect(plot->legend->item(lastItem), &QCPAbstractLegendItem::selectionChanged, [p](auto state)
    {
        p->graph(p->graphCount() - 1)->setVisible(!state);
    });
}


void CPlotter::setAllMarkerHideFunction(QCustomPlot* plot, const QVector <qint32>& indexes)
{
    for (int i = 0; i < plot->legend->itemCount(); i++)
    {
        plot->legend->item(i)->setSelectable(true);
        auto index = indexes[i];
        connect(plot->legend->item(i), &QCPAbstractLegendItem::selectionChanged, [plot, index](auto state)
        {
            plot->graph(index)->setVisible(!state);
        });
    }
}

void CPlotter::resetOptions()
{
    axisXRange = QPair <double, double> (0,0);
    axisYRange = QPair <double, double> (0,0);
    axisXCenter = -1;
    axisYCenter = -1;
    adaptiveSampling = true;
    title.clear();
    interactions = QCP::iRangeDrag | QCP::iRangeZoom| QCP::iSelectLegend;
    axisXName.clear();
    axisXFormat.clear();
    XTickCount = 10;
    XTickOrigin = 0;
    axisYName.clear();
    axisYFormat.clear();
    YTickCount = 5;
    YTickOrigin = 0;
    useOpenGL = false;
    pColor.isSet = false;

}

bool CPlotter::setRange()
{

    bool used = false;
    if (!(qFuzzyCompare(axisXRange.first, 0) && qFuzzyCompare(axisXRange.second, 0)))
    {
        if (!qFuzzyCompare(axisXCenter, -1))
        {
            plot->xAxis->setRange(axisXCenter, axisXRange.first * 2, Qt::AlignCenter);
        }
        else
        {
            plot->xAxis->setRange(QCPRange(axisXRange.first, axisXRange.second));
        }
        used = true;
    }
    if (!(qFuzzyCompare(axisYRange.first, 0) && qFuzzyCompare(axisYRange.second, 0)))
    {
        if (!qFuzzyCompare(axisYCenter, -1))
        {
            plot->yAxis->setRange(axisYCenter, axisYRange.first * 2, Qt::AlignCenter);
        }
        else
        {
            plot->yAxis->setRange(QCPRange(axisYRange.first, axisYRange.second));
        }
        used = true;
    }
    return used;
}

void CPlotter::setPlotOptions()
{
    setSaveMenu();
    setRectangleZoomComb();

    plot->setInteractions(interactions);
    plot->setMultiSelectModifier(modifier);
    plot->setOpenGl(useOpenGL);
    plot->xAxis->setLabel(axisXName);
    plot->xAxis->setRangeReversed(xAxisReversed);
    plot->yAxis->setLabel(axisYName);
    plot->yAxis->setRangeReversed(yAxisReversed);
    plot->yAxis->setNumberPrecision(8);
    plot->legend->setVisible(getUseLegend());


    if (!qFuzzyCompare(axisXCenter, -1) && !qFuzzyCompare(axisYCenter, -1))
    {
        plot->setInteraction(QCP::iRangeZoom, false);
        auto tmpPlot = plot;
        connect(plot, &QCustomPlot::mouseWheel, [tmpPlot](QWheelEvent* event)
        {
            if (!tmpPlot->interactions().testFlag(QCP::iRangeZoom))
            {
                if (event->angleDelta().y() < 0)
                {
                    tmpPlot->xAxis->scaleRange(1.05);
                    tmpPlot->yAxis->scaleRange(1.05);
                }
                else
                {
                    tmpPlot->xAxis->scaleRange(0.95);
                    tmpPlot->yAxis->scaleRange(0.95);
                }
                tmpPlot->replot();
            }

        });
    }
    else
    {
        plot->setInteraction(QCP::iRangeZoom, true);
    }

    if (XTickCount)
    {
        plot->xAxis->ticker()->setTickCount(XTickCount);
    }
    if (qFuzzyCompare(XTickOrigin, 0))
    {
        plot->xAxis->ticker()->setTickOrigin(XTickOrigin);
    }
    if (YTickCount)
    {
        plot->yAxis->ticker()->setTickCount(YTickCount);
    }
    if (qFuzzyCompare(YTickOrigin, 0))
    {
        plot->yAxis->ticker()->setTickOrigin(YTickOrigin);
    }
    plot->xAxis->ticker()->setTickStepStrategy(QCPAxisTicker::TickStepStrategy::tssMeetTickCount);
    plot->yAxis->ticker()->setTickStepStrategy(QCPAxisTicker::TickStepStrategy::tssMeetTickCount);

    if (dynamic_cast <QCPAxisTickerDateTime*> (plot->xAxis->ticker().data()))
    {
        dynamic_cast <QCPAxisTickerDateTime*> (plot->xAxis->ticker().data())->setDateTimeFormat(axisXFormat);
    }

    if (dynamic_cast <QCPAxisTickerDateTime*> (plot->yAxis->ticker().data()))
    {
        dynamic_cast <QCPAxisTickerDateTime*> (plot->yAxis->ticker().data())->setDateTimeFormat(axisYFormat);
    }

    Q_ASSERT(plot->plotLayout()->elementCount() <= 2);

    if (plot->plotLayout()->elementCount() == 2)
    {
        static_cast <QCPTextElement*> (plot->plotLayout()->elementAt(0))->setText(title);
    }
    else
    {
        QCPTextElement* titleItem = new QCPTextElement(plot);
        titleItem->setText(title);
        titleItem->setFont(QFont("sans", 8, QFont::Bold));
        plot->plotLayout()->insertRow(0);
        plot->plotLayout()->addElement(0, 0, titleItem);
    }


}




void CPlotter::synchronizeLegends(const QVector <QCustomPlot*>& set)
{
    qint32 itemsCount = set.first()->legend->itemCount();
    for (auto& i : set)
    {
        if (i->legend->itemCount() != itemsCount)
        {
            throw CPlotterException("Легенды графиков не совпадают. Невозможно синхронизировать");
        }
    }
    QVector <qint32> indexes;
    for (int i = 0; i < itemsCount; i++)
    {
        indexes.append(i);
    }
    synchronizeLegendsPrivate(set, indexes);
}

void CPlotter::synchronizeLegendsPrivate(const QVector <QCustomPlot*>& set, const QVector<qint32>& indexes)
{

    for (const auto& plot : set)
    {
        QVector <QCustomPlot*> subset;
        for (int i = 0; i < set.size(); i++)
        {
            if(set[i] != plot)
            {
                subset.append(set[i]);
            }
        }
        for (int i = 0; i < plot->legend->itemCount(); i++)
        {
            plot->legend->item(i)->setSelectable(true);
            qint32 index = indexes[i];
            connect(plot->legend->item(i), &QCPAbstractLegendItem::selectionChanged, [subset, i, index] (auto state)
            {
                for (const auto& depPlot : subset)
                {
                    depPlot->graph(index)->setVisible(state);
                    depPlot->legend->item(i)->setSelected(state);
                    depPlot->replot();
                }
            });
        }
    }
}

void CPlotter::addLines(const QVector<QPointF>& startPoints, const QVector<QPointF>& endPoints, QCPLineEnding::EndingStyle begin, QCPLineEnding::EndingStyle end)
{
    if (!plotIsSetted())
    {
        return;
    }

    if (startPoints.size() != endPoints.size())
    {
        throw CPlotterException("Не совпадает количество координат концов и начал линий");
    }

    for (int i = 0; i < startPoints.size(); i ++)
    {
        if (startPoints[i] != endPoints[i])
        {
            QCPItemLine* line = new QCPItemLine(plot);
            line->setHead(begin);
            line->setTail(end);
            line->start->setCoords(startPoints[i].x(),startPoints[i].y());
            line->end->setCoords(endPoints[i].x(),endPoints[i].y());
        }
    }
}

CPlotter::~CPlotter()
{

}
