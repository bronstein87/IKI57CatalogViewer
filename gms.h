#ifndef GMS_H
#define GMS_H
#include <QDebug>

class GMS
{
public:
        explicit GMS(double);
        explicit GMS(const qint32, const qint32, const double);
        explicit GMS();
        GMS(const GMS&)                   =default;
        GMS(GMS&& )                       =default;
        GMS& operator=(const GMS&)        =default;
        GMS& operator=(GMS&&)             =default;
        ~GMS()                            =default;
        double transToGrad();
        double transToRad();
        qint32 getGradus() const {
            return gradus;
        }
        qint32 getMinutes() const {
            return minutes;
        }
        double getSeconds() const {
            return seconds;
        }
        void setAngleValue(double);
        void setGradus(const qint32);
        void setMinutes(const qint32);
        void setSeconds(const double);

        bool isEmpty() const;
        static bool checkStringGMSAngle(const QString& );
        friend QDebug operator << (QDebug d, const GMS &data);
private:
    qint32 gradus;
    qint32 minutes;
    double seconds;





};

#endif // GMS_H
