#ifndef CXYPLOTTER_H
#define CXYPLOTTER_H

#include <customplot/cplotter.h>
#include <goodcolors.h>

enum PlotType
{
   DEFAULT,
   DATETIME,
   STABLE
};

class CXYPlotter final : public CPlotter
{
Q_OBJECT
public:

    explicit CXYPlotter(QCustomPlot* plot = nullptr, QObject* parent = nullptr);

    CXYPlotter(const CXYPlotter&)                   =delete;

    CXYPlotter(CXYPlotter&&)                        =delete;

    CXYPlotter& operator = (const CXYPlotter&)      =delete;

    CXYPlotter& operator = (CXYPlotter&&)           =delete;

     ~CXYPlotter()                                  =default;

    virtual void resetOptions() override;

    void addPoint (const double& key, const double& value, PlotType type, qint32 seriesPos);
    // объединить в одну функцию с DateTimeGraph, настраивать график в зависимости от ENUM
    void addDefaultGraph(const QVector<double>& firstDataVec, const QVector<double>& secondDataVec, bool sorted = false, bool syncAfter = false);

    void addDateTimeGraph(const QVector<double>& firstDataVec,const QVector<double>& , bool sorted = false, bool syncAfter = false);

    void addHistogramm(const QVector <double>& values, double step, bool groupOffset);

    void updateGraph(const QVector<double>& firstDataVec, const QVector<double>& secondDataVec, int number, bool sorted = false);

    QCPItemTracer* setTracer(int numberGraph, double pointX, bool selectable = false, QCPItemTracer::TracerStyle style = QCPItemTracer::TracerStyle::tsSquare);



    void setStyle(const QCPGraph::LineStyle& _lStyle, const QCPScatterStyle& _scStyle) noexcept
    {
        lStyle = _lStyle;
        scStyle = _scStyle;
    }

    void setGraphName(const QString& _name) noexcept
    {
        name = _name;
    }

    QString getGraphName() const noexcept
    {
        return name;
    }

    QPair <QCPGraph::LineStyle,QCPScatterStyle> getStyle() const noexcept
    {
        return QPair <QCPGraph::LineStyle, QCPScatterStyle> (lStyle, scStyle);
    }


private:

    void makeMainPlotOperations(const QVector <double>& secondDataVec, const QVector <double>& firstDataVec, bool sorted, bool syncAfter = false);

    QString name;
    QCPGraph::LineStyle lStyle = QCPGraph::lsLine;
    QCPScatterStyle  scStyle = QCPScatterStyle(QCPScatterStyle::ssNone);

};

#endif // CXYPLOTTER_H
