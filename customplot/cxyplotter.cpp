#include "cxyplotter.h"



CXYPlotter::CXYPlotter(QCustomPlot* plot, QObject* parent) : CPlotter(plot, parent)
{

}

void CXYPlotter::resetOptions()
{
    CPlotter::resetOptions();
    lStyle = QCPGraph::lsLine;
    scStyle = QCPScatterStyle(QCPScatterStyle::ssNone);
    name.clear();
}



void CXYPlotter::makeMainPlotOperations(const QVector<double>& secondDataVec, const QVector<double>& firstDataVec, bool sorted, bool syncAfter)
{
    plot->setAutoAddPlottableToLegend(getUseLegend());
    plot->addGraph();
    plot->graph()->setData(secondDataVec, firstDataVec, sorted);
    plot->graph()->setLineStyle(lStyle);
    plot->graph()->setScatterStyle(scStyle);
    plot->graph()->setName(name);
    plot->graph()->setAdaptiveSampling(getAdaptiveSampling());
    plot->graph()->setSelectable(getSelectionType());

    if (!syncAfter)
    {
        if (getUseLegend())
        {
            setMarkerHideFunction();
        }
        const auto& colors = GoodColors::Instance();
        if (pColor.isSet)
        {
            plot->graph()->setPen(pColor.color); // нужно исправить потом
        }
        else
        {
            plot->graph()->setPen(colors.getColor(plot->graphCount() - 1)); // нужно исправить потом
        }
        if (needRescale)
        {
            if (!setRange())
            {
                plot->rescaleAxes();
            }
        }
        if (getAutoReplot())
        {
            plot->replot();
        }
    }
    else
    {
        plot->setAutoAddPlottableToLegend(false);
        plot->graph()->setVisible(false);
    }


}

void CXYPlotter::addPoint(const double& key, const double& value, PlotType type, qint32 seriesPos)
{
    if (!plotIsSetted())
        return;

    if (plotIsEmpty())
    {
        switch(type)
        {
            case DATETIME:
            {
                auto dateTicker = QSharedPointer <QCPAxisTickerDateTime>::create();
                plot->xAxis->setTicker(dateTicker);
                break;
            }

            case STABLE:
            {
                auto fixedTicker = QSharedPointer <QCPAxisTickerFixed>::create();
                plot->xAxis->setTicker(fixedTicker);
                fixedTicker->setTickStep(1.0);
                break;
            }
            case DEFAULT:
                break;

        }
        setPlotOptions();
        setPlotterRescale();
    }
    if (plot->graphCount() <= seriesPos)
    {
        plot->addGraph();
        plot->graph(seriesPos)->setLineStyle(lStyle);
        plot->graph(seriesPos)->setScatterStyle(scStyle);
        plot->graph(seriesPos)->setName(name);
        plot->graph(seriesPos)->setAdaptiveSampling(getAdaptiveSampling());
    }
    plot->graph(seriesPos)->addData(key, value);

}


void CXYPlotter::addDefaultGraph(const QVector<double>& firstDataVec, const QVector<double>& secondDataVec, bool sorted, bool syncAfter)
{
    if (!plotIsSetted())
        return;

    if (plotIsEmpty())
    {
        setPlotOptions();
        setPlotterRescale();
    }
    makeMainPlotOperations(secondDataVec, firstDataVec, sorted, syncAfter);
}

void CXYPlotter::addDateTimeGraph(const QVector<double>& firstDataVec, const QVector<double>& secondDataVec, bool sorted, bool syncAfter)
{
    if (!plotIsSetted())
        return;

    if (plotIsEmpty())
    {
        auto dateTicker = QSharedPointer <QCPAxisTickerDateTime>::create();
        plot->xAxis->setTicker(dateTicker);
        setPlotOptions();
        setPlotterRescale();
    }
    makeMainPlotOperations(secondDataVec, firstDataVec, sorted, syncAfter);
}

void CXYPlotter::addHistogramm(const QVector <double>& values, double step, bool groupOffset)
{
    plot->clearPlottables();
    plot->clearItems();
    setPlotOptions();
    auto minmax = std::minmax_element(values.begin(), values.end());
    qint32 begin = (qint32)(*minmax.first / step);
    qint32 end = (qint32)(*minmax.second / step) + 1;
    QVector <double> count(end - begin, 0);
    for (const auto& i : values)
    {
        ++count[(i / step) - begin];
    }

    QVector<double> ticks;
    QVector<QString> labels;
    qint32 offset = groupOffset ? 1 : 0;
    for (qint32 i = begin + offset; i < end + offset; ++i)
    {
        ticks.append(i);
        labels.append(QString::number(i * step));
    }
    QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
    textTicker->addTicks(ticks, labels);
    plot->xAxis->setTicker(textTicker);
    plot->xAxis->setTickLabelRotation(60);
    plot->xAxis->setSubTicks(false);
    minmax = std::minmax_element(count.begin(), count.end());
    plot->yAxis->setRange(*minmax.first, *minmax.second);

    QCPBars* hist = new QCPBars(plot->xAxis, plot->yAxis);
    hist->setData(ticks, count);
    plot->rescaleAxes();
    plot->replot();
}

void CXYPlotter::updateGraph(const QVector<double>& firstDataVec , const QVector<double>& secondDataVec, int number, bool sorted)
{
    if (!plotIsSetted())
        return;
    const auto& colors = GoodColors::Instance();
    plot->graph(number)->setLineStyle(lStyle);
    plot->graph(number)->setPen(colors.getColor(number));
    plot->graph(number)->setScatterStyle(scStyle);
    plot->graph(number)->setData(firstDataVec, secondDataVec, sorted);
    plot->replot();
}

QCPItemTracer* CXYPlotter::setTracer(int numberGraph, double pointX, bool selectable, QCPItemTracer::TracerStyle style)
{
    QCPItemTracer* tr = new QCPItemTracer(plot);
    tr->setSelectable(selectable);
    tr->setSize(10);
    tr->setStyle(style);
    // tr->setBrush(QBrush(QColor(255, 0, 0)));
    tr->setGraph(plot->graph(numberGraph));
    tr->setGraphKey(pointX);
    if (getAutoReplot())
    {
        plot->replot();
    }
    return tr;
}


