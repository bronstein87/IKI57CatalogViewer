#ifndef GOODCOLORS_H
#define GOODCOLORS_H
#include <QVector>
#include <QColor>

class GoodColors
{
private:

    GoodColors() = default;

    GoodColors(const GoodColors&) = delete;

    GoodColors(GoodColors&& ) = delete;

    GoodColors& operator=(const GoodColors&) = delete;

    GoodColors& operator=(GoodColors&&) = delete;

    ~GoodColors() = default;

public:

    const static GoodColors& Instance() noexcept
    {
        static GoodColors g;
        return g;
    }

    QColor getColor(qint32 i) const noexcept
    {
        if (i > goodColors.size() - 1)
        {
            return goodColors[i % goodColors.size()];
        }
        return goodColors[i];
    }


private:

    QVector <QColor> goodColors
    {
        QRgb(0x209fdf),
        QRgb(0x99ca53),
        QRgb(0xf6a625),
        QRgb(0x6d5fd5),
        QRgb(0xbf593e),
        QRgb(0x01ff70),
        QRgb(0x7fdbff),
        QRgb(0x85144b),
        QRgb(0xffdc00),
        QRgb(0xff4136),
        QRgb(0x0074d9)
    };
};

#endif // GOODCOLORS_H
