#ifndef CPLOTTER_H
#define CPLOTTER_H
#include <customplot/qcustomplot.h>
#include <goodcolors.h>
#include <algorithm>
using namespace std;



class CPlotterException : public std::exception
{
public:
    CPlotterException(const std::string &&whatStr) noexcept : whatStr(std::move(whatStr)) { }
    CPlotterException(const std::string &whatStr) noexcept : whatStr(whatStr) { }
    ~CPlotterException() noexcept = default;

    const char* what() const noexcept override
    {
        return whatStr.c_str();
    }

private:
    std::string whatStr;
};

class CPlotter : public QObject
{

public:

    explicit CPlotter(QCustomPlot* _plot = nullptr, QObject* parent = nullptr);

    CPlotter(const CPlotter&)                   =delete;

    CPlotter(CPlotter&& )                       =delete;

    CPlotter& operator=(const CPlotter&)        =delete;

    CPlotter& operator=(CPlotter&&)             =delete;

    virtual ~CPlotter() = 0 ;


    void addLines(const QVector <QPointF>& startPoints, const QVector <QPointF>& endPoints,
                  QCPLineEnding::EndingStyle begin = QCPLineEnding::EndingStyle::esNone ,
                  QCPLineEnding::EndingStyle end = QCPLineEnding::EndingStyle::esNone);

    void addEmptyGraphs(const QStringList& names, qint32 count);

    template <typename SortableAttribute>
    static void synchronizeGraph(QList <SortableAttribute> attributes);

    virtual void resetOptions();

    void setInteractions(const QCP::Interactions & _interactions) noexcept
    {
        interactions = _interactions;
    }

    QCP::Interactions getIteractions() const noexcept
    {
        return interactions;
    }

    void setMultiSelectModifier(Qt::KeyboardModifier _modifier) noexcept
    {
        modifier = _modifier;
    }

    Qt::KeyboardModifier getMultiSelectModifier() const noexcept
    {
        return modifier;
    }

    void setTitle(const QString& _title) noexcept
    {
        title = _title;
    }

    QString getTitle() const noexcept
    {
        return title;
    }

    void setAxisXName(const QString& _axisXName) noexcept
    {
        axisXName = _axisXName;
    }

    QString getAxisXName() const noexcept
    {
        return axisXName;
    }

    void setAxisXFormat(const QString& _axisXFormat) noexcept
    {
        axisXFormat = _axisXFormat;
    }

    QString getAxisXFormat() const noexcept
    {
        return axisXFormat;
    }

    void setAxisYName(const QString& _axisYName) noexcept
    {
        axisYName = _axisYName;
    }

    QString getAxisYName() const noexcept
    {
        return axisYName;
    }

    void setAxisYFormat(const QString& _axisYFormat) noexcept
    {
        axisYFormat = _axisYFormat;
    }

    QString getAxisYFormat() const noexcept
    {
        return axisYFormat;
    }


    void setYTickCount(const qint32 _YTickCount) noexcept
    {
        YTickCount = _YTickCount;
    }

    qint32 getYTickCount() const noexcept
    {
        return YTickCount;
    }

    void setXTickCount(const qint32 _XTickCount) noexcept
    {
        XTickCount = _XTickCount;
    }

    qint32 getXTickCount() const noexcept
    {
        return XTickCount;
    }

    void setRangeAxisX(double min,double max) noexcept
    {
        axisXRange.first = min;
        axisXRange.second = max;
    }

    void setRangeAxisY(double min, double max) noexcept
    {
        axisYRange.first = min;
        axisYRange.second = max;
    }


    void setAxisXCenter(double center) noexcept
    {
        axisXCenter = center;
    }

    void setAxisYCenter(double center) noexcept
    {
        axisYCenter = center;
    }


    void setAdaptiveSampling(bool state) noexcept
    {
        adaptiveSampling = state;
    }

    bool getAdaptiveSampling() const noexcept
    {
        return adaptiveSampling;
    }

    void setUseOpenGL(bool _use) noexcept
    {
        useOpenGL = _use;
    }

    bool getUseOpenGL() const noexcept
    {
        return useOpenGL;
    }

    bool plotIsEmpty() noexcept
    {
        return plotIsSetted()? !plot->graphCount() : true;
    }

    bool plotIsSetted() noexcept
    {
        return plot != nullptr;
    }

    void setXTickOrigin (double origin) noexcept
    {
        XTickOrigin = origin;
    }
    double getXTickOrigin() const noexcept
    {
        return XTickOrigin;
    }

    void setYTickOrigin (double origin) noexcept
    {
        YTickOrigin = origin;
    }

    double getYTickOrigin() const noexcept
    {
        return YTickOrigin ;
    }

    void setYTickCount(double origin) noexcept
    {
        YTickOrigin = origin;
    }

    void setPlot(QCustomPlot* _plot) noexcept
    {
        plot = _plot;
    }

    void setSelectable (QCP::SelectionType type) noexcept
    {
        selectionType = type;
    }

    QCP::SelectionType getSelectionType() const noexcept
    {
        return selectionType;
    }

    void setUseLegend (bool use) noexcept
    {
        useLegend = use;
    }

    bool getUseLegend() const noexcept
    {
        return useLegend;
    }

    void setAutoReplot(bool _autoReplot) noexcept
    {
        autoReplot = _autoReplot;
    }

    bool getAutoReplot() const noexcept
    {
        return autoReplot;
    }

    void setCustomColor(QColor color)
    {
        pColor.color = color;
        pColor.isSet = true;
    }
    void removeCustomColor()
    {
        pColor.isSet = false;
    }

    void setXAxisReversed(bool f) {xAxisReversed = f;}

    void setYAxisReversed(bool f) {yAxisReversed = f;}

    void setNeedRescale(bool f) {needRescale = f;}

    static void synchronizeLegends(const QVector <QCustomPlot*>& set);

    template <typename SortableAttribute>
    static void synchronizeGraphs(const QList <SortableAttribute>& attributes, const QVector <QCustomPlot*>& set, bool syncLegends = false);

    template <typename SortableAttribute>
    static void synchronizeGraph(const QList <SortableAttribute>& attributes,  QCustomPlot* set);


protected:
    struct CustomColor
    {
        QColor color;
        bool isSet;
    };

    template <typename SortableAttribute>
    static QVector <qint32> synchronizeGraphPrivate(const QList <SortableAttribute>& attributes, QCustomPlot* plot);

    static void setAllMarkerHideFunction(QCustomPlot* plot, const QVector<qint32>& indexes);

    static void synchronizeLegendsPrivate(const QVector <QCustomPlot*>& set, const QVector<qint32>& indexes);

    void setPlotOptions();

    void setPlotterRescale();

    void setSaveMenu();

    void setRectangleZoomComb();

    void setMarkerHideFunction();

    bool setRange();

    QCustomPlot* plot;
    QPair <double, double> axisXRange = QPair <double, double> (0, 0);
    double axisXCenter = -1;
    bool xAxisReversed = false;

    QPair <double, double> axisYRange = QPair <double, double> (0, 0);
    double axisYCenter = -1;
    bool yAxisReversed = false;

    CustomColor pColor;
    bool needRescale = true;

private:

    QString title;
    QString axisXName;
    QString axisXFormat;
    qint32 XTickCount = 10;
    double XTickOrigin = 0;


    QString axisYName;
    QString axisYFormat;
    qint32 YTickCount = 5;
    double YTickOrigin = 0;


    bool useOpenGL = false;
    bool adaptiveSampling = true;
    QCP::Interactions  interactions = QCP::iRangeDrag | QCP::iRangeZoom| QCP::iSelectLegend| QCP::iMultiSelect;
    Qt::KeyboardModifier modifier = Qt::NoModifier;
    QCP::SelectionType selectionType = QCP::SelectionType::stWhole;
    bool useLegend = true;
    bool autoReplot = true;


};


template <typename SortableAttribute>
void CPlotter::synchronizeGraphs(const QList <SortableAttribute>& attributes, const QVector <QCustomPlot*>& set, bool syncLegends)
{
    QVector <qint32> indexes;
    for(const auto& plot : set)
    {
        indexes = synchronizeGraphPrivate(attributes, plot); // исправить
    }
    if (syncLegends)
    {
        synchronizeLegendsPrivate(set, indexes);
    }
}

template <typename SortableAttribute>
QVector <qint32> CPlotter::synchronizeGraphPrivate(const QList <SortableAttribute>& attributes, QCustomPlot* plot)
{
    if (attributes.size() != plot->graphCount())
        throw CPlotterException("Признаки и число графиков не совпадают.");

    QMap < SortableAttribute, QCPGraph* > map;
    for (int i = 0; i < attributes.size(); i++)
    {
        map.insert(attributes[i], plot->graph(i));
    }

    int j = 0;
    const auto& colors = GoodColors::Instance();
    QMapIterator<SortableAttribute, QCPGraph*> i(map);
    plot->legend->clear();
    QVector <qint32> indexes;
    while (i.hasNext())
    {
        i.next();
        i.value()->setPen(colors.getColor(j++));
        i.value()->addToLegend();
        i.value()->setVisible(true);
        indexes.append(attributes.indexOf(i.key()));
    }
    setAllMarkerHideFunction(plot, indexes);
    plot->rescaleAxes();
    plot->replot();
    return indexes;
}


template <typename SortableAttribute>
void CPlotter::synchronizeGraph(const QList <SortableAttribute>& attributes, QCustomPlot* plot)
{
    synchronizeGraphPrivate(attributes, plot);
}


#endif  // CPLOTTER_H


