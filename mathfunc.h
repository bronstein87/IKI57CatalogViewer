#ifndef MATHFUNC_H
#define MATHFUNC_H
#include <utility>
#include <iterator>
#include <cmath>
#include <QtGlobal>
#include <QDateTime>
#include <QPointF>
#include <QVector>


typedef int (*CorrectBoardQuatResurs)(wchar_t *patch_cat, /*Имя папки c бортовыми каталогами*/
                                wchar_t *patch_dtmi, /*Имя папки для создания файлов с тестовой информацией, если NULL - файлы не создаются*/
                                double ArrBokz[][8], /*Массив кватернионов*/
unsigned int &nBokz, /*Число измерений кватернионов*/
unsigned int nBokzMax, /*Макс. размер массива кватернионов*/
float ArrLoc[][77], /*Массив локализованных объектов*/
unsigned int nLoc, /*Размер массива локализованных объектов*/
const double ArrVect[][18], /*Массив ПДСМ*/
unsigned int nVect, /*Размер массива ПДСМ*/
unsigned int &CurrentIndex, /*Текущий индекс массива кватернионов*/
unsigned char &Stop); /*Признак останова программы*/

typedef int (*CorrectBoardQuatBARS)(wchar_t *patch_cat, /*Имя папки c бортовыми каталогами*/
                                wchar_t *patch_dtmi, /*Имя папки для создания файлов с тестовой информацией, если NULL - файлы не создаются*/
                                double ArrBokz[][13], /*Массив кватернионов*/
unsigned int &nBokz, /*Число измерений кватернионов*/
unsigned int nBokzMax, /*Макс. размер массива кватернионов*/
float ArrLoc[][77], /*Массив локализованных объектов*/
unsigned int nLoc, /*Размер массива локализованных объектов*/
const double ArrVect[][18], /*Массив ПДСМ*/
unsigned int nVect, /*Размер массива ПДСМ*/
unsigned int &CurrentIndex, /*Текущий индекс массива кватернионов*/
unsigned char &Stop); /*Признак останова программы*/

enum AxisType
{
    xAxis,
    yAxis,
    zAxis
};

struct RecognizedInfo
{
    double distance = 0;
    int numInCat = 0;
    bool checked = false;
    bool recognized = false;
};


namespace BOKZMath {

constexpr const double mSecsInSec = 1000;
constexpr const double secsInDay = 86400;
constexpr const double mSecsInDay = 86400000;
constexpr const double radToAngularMin = 3437.7467747131374;
constexpr const double radToDegrees = 57.29577957855229;
constexpr const double degreesToRad = 0.0174532925;
constexpr const double radToSec = 180. * 3600. / M_PI;
constexpr const double secToRad = 1. / radToSec;
constexpr const double julianZeroDate = 2451545.;
constexpr const double countOfDaysInCentury = 36525.;
constexpr const double secOfTimeToArcSec = 360. / 86400. * 3600.;
constexpr const double arcMSecToDegree =  1. / (3600 * 1000);
constexpr const double arcSecToDegree =  1. / 3600;


void correctLocArray(float locArray[16][4]) noexcept;

void correctAngularVArray(float Wo[3]) noexcept;

void swapShort(void* value) noexcept;

double calculateJulianDate(const QDateTime& dateTime) noexcept;

double calculateJulianCenturies(const QDateTime& dateTime) noexcept;

void multMatrixVector(const double M[3][3], const double V[3], double VR[3]) noexcept;

double calculateDatePr(const QDateTime& dateTime, int timePr) noexcept;

double toTDateTimeValue(const QDateTime& datetime) noexcept;

QDateTime fromTDateTime(double value) noexcept;

/*смотри http://law.rufox.ru/view/standarti/1672.htm*/
void calculateMoonDirection(double JD_DEV, double* pMoonI) noexcept;

void calculateSunDirection(double JD_DEV, double* pSunI) noexcept;

double starsTime2(double JD) noexcept;

void getGSKISKMatrix(double JD, double MG[3][3]) noexcept;

void  transSpeedGSKtoISK(double JD, double CG[3], double VG[3], double VI[3]) noexcept;

void  transCoordsGSKtoISK(double JD, double CG[3], double CI[3]) noexcept;

void quatToMatr(const double Quat[], double mOrnt[3][3]) noexcept;

double atan2m(double yf,double xf) noexcept;

void matrToEkvA(double mOrnt[3][3], double& al, double& dl, double& Az, AxisType axis = AxisType::zAxis) noexcept;

void quatToEkvA(const double Quat[4], double EkvA[3], AxisType axis = AxisType::zAxis) noexcept;

double acosm(double xf) noexcept;

double asinm(double x) noexcept;

void multiplyMatrix(const double Matr1[3][3],const double Matr2[3][3], double Matr[3][3]) noexcept;

void getAngularDisplacementFromOrientMatr(const double mOrnt_pr[3][3],const double mOrnt[3][3], double Wop [3]) noexcept;

double calculateAngleAxis(const double quat1[], const double quat2[], AxisType axis) noexcept;

QVector <QPointF> createHistogramm(size_t histSize, float shiftRange, const QVector <float>& firstX,const QVector <float>& firstY,const QVector <float>& secondX,const QVector <float>& secondY) noexcept;

QVector <QPointF> createHistogramm(size_t histSize, float shiftRange, const QVector<QPointF>& firstCoords, const QVector<QPointF>& secondCoords) noexcept;

qint64 roundUp(qint64 numToRound, qint64 multiple) noexcept;

QDateTime timePrToDateTime(const double timePr, const QDateTime& timePrStartData) noexcept;

QDateTime timePrToDateTime(const qint32 timePr, const QDateTime& timePrStartData) noexcept;

QDateTime timePrToDateTime(const quint64 timePr, const QDateTime& timePrStartData) noexcept;

QVector< QVector<double> > calcTransitionMatrix(double pointAlpha, double pointBeta, double pointAzimut) noexcept;

void calculateDirectionVector(double alpha, double delta, double& l, double& m, double& n);

void calcTransitionMatrix(double pointAlpha, double pointBeta, double pointAzimut, double mOrnt[3][3]) noexcept;

double calculateScalarProduct(double lOz, double lSt, double mOz, double mSt, double nOz, double nSt) noexcept;

double calculateScalarProductAngles(double anglesFirst[3], double anglesSecond[3]);

void calculateAlphaDelta(double l, double m, double n, double& alpha, double& delta) noexcept;

QVector <qint32> firstMinDistanceTable(RecognizedInfo** distMatrix, qint32 countOfMins, qint32 objectIndex, qint32 size);

void calculateLMNImage(double x, double y, double focus, double lmn[3]) noexcept;

double sqrtm(double x);

int LUPDecompose(double** A, int N, double Tol, int* P) noexcept;

double LUPDeterminant(double* *A, int* P, int N) noexcept;

void LUPInvert(double** A, int* P, int N, double** IA) noexcept;

double getDet(int nElem, double** DRVM);

void invertMatrix(double** DRVM, double** DRVM_1, int nElem);

int minorM(int z, int x, int N, double** A, double** C) noexcept;

double calculateDistorsio(double coordC, double coordA, double coordB, const QList<double>& distorsioCoef) noexcept;

double calculateDistorsioDelta(double coordA, double coordB,  const QList<double>& distorsioCoef) noexcept;

bool checkQuatNorm(double q[4]) noexcept;

void rotateOY(double alpha, double mIn[3][3], double mOut[3][3]) noexcept;

void rotateOZ(double alpha, double mIn[3][3], double mOut[3][3]) noexcept;

void rotateOX(double alpha, double mIn[3][3], double mOut[3][3]) noexcept;


template <class InputIterator, class Value, class UnaryOperation>
std::pair <Value, Value> calculateMeanStdDv (InputIterator first, InputIterator last, Value init, UnaryOperation extractWtC)
{
    if (first == last) return std::pair <Value, Value> (extractWtC(*first), Value());

    Value dispersio = 0;
    for (InputIterator i = first; i < last; i++)
    {
        init += extractWtC(*i);
        dispersio += pow(extractWtC(*i), 2);
    }
    auto count = std::distance(first,last);
    Value mean = init / count;
    dispersio = (dispersio / count) - pow(mean, 2);
    if(fabs(dispersio) < 1.0e-20) dispersio = 0;

    return std::pair <Value,Value> (mean, sqrt(dispersio));
}


template <class InputIterator, class Value>
std::pair<Value, Value> calculateMeanStdDv (InputIterator first, InputIterator last, Value init) noexcept
{
    if (first == last) return std::pair <Value, Value> (*first, Value());

    Value dispersio = 0;
    for (InputIterator i = first; i < last; i++)
    {
        init += *i;
        dispersio += pow(*i, 2);
    }
    auto count = std::distance(first,last);
    Value mean = init / count;
    dispersio = (dispersio / count) - pow(mean, 2);
    if(fabs(dispersio) < 1.0e-20) dispersio = 0;

    return std::pair <Value,Value> (mean, sqrt(dispersio));

}


template <class InputIterator, class T, class UnaryOperation>
T calculateMean (InputIterator first, InputIterator last, T init, UnaryOperation extractWtC) noexcept
{
    if (first == last) return extractWtC(first);

    for (InputIterator i = first;i < last; i++)
    {
        init += extractWtC(i);
    }
    auto count = std::distance(first,last);
    return init/count;
}

template <class InputIterator, class T>
T calculateMean (InputIterator first, InputIterator last, T init) noexcept
{
    if (first == last) return *first;

    for (InputIterator i = first;i < last; i++)
    {
        init += *i;
    }
    auto count = std::distance(first,last);
    return init/count;
}


/*функция считающая пересечения в векторах, и оставляющая только общие значения по времени для каждого*/
template <typename Data, typename Functor>
void setIntersectionData(QVector <Data>& intersData, Functor&& functor)
{

    if (intersData.size() <= 1)
    {
        return;
    }
    for (qint32 i = 1; i < intersData.size(); i++)
    {
        Data intersectedVector;// вектор, который будет содержать непересекающиеся значения, его будем присваивать первому вектору
        std::set_intersection (intersData[0].begin(), intersData[0].end(), intersData[i].begin(), intersData[i].end(),
                std::back_inserter(intersectedVector), functor);
        intersData.first() = std::move(intersectedVector);
    }

    for (qint32 i = 1; i < intersData.size(); i++)
    {
        Data intersectedVector;
        std::set_intersection (intersData[i].begin(), intersData[i].end(), intersData.first().begin(), intersData.first().end(),
                               std::back_inserter(intersectedVector), functor);
        intersData[i] = std::move(intersectedVector);
    }
}
template <typename Matrix>
std::pair <qint32, qint32> matrixMax(const Matrix& matrix) noexcept
{
    if (matrix.size() < 1)
    {
        return std::pair <qint32, qint32> (0, 0);
    }
    qint32 row = 0;
    qint32 column = 0;
    auto maxMatrix = decltype(matrix[0][0]){0};
    for (int i = 0 ; i < matrix.size(); i++)
    {
        for (int j = 0 ; j < matrix[i].size(); j++)
        {
            if (maxMatrix < matrix[i][j])
            {
                maxMatrix = matrix[i][j];
                row = i;
                column = j;
            }
        }
    }
    return std::pair <qint32, qint32> (row, column);
}

template <typename Matrix>
std::pair <qint32, qint32> matrixMin(const Matrix& matrix) noexcept
{
    if (matrix.size() < 1)
    {
        return std::pair <qint32, qint32> (0, 0);
    }
    qint32 row = 0;
    qint32 column = 0;
    auto minMatrix = matrix[0][0];

    for (int i = 0 ; i < matrix.size(); i++)
    {
        for (int j = 0 ; j < matrix[i].size(); j++)
        {
            if (minMatrix < matrix[i][j])
            {
                minMatrix = matrix[i][j];
                row = i;
                column = j;
            }
        }
    }
    return std::pair <qint32, qint32> (row, column);
}

template <class InputIterator>
QVector <qint32> firstMinElements (InputIterator first, InputIterator last, qint32 countOfMins)
{
    QVector <float> vec(countOfMins, std::numeric_limits <float>::max());
    QVector <qint32> minIndexes(countOfMins, std::numeric_limits <qint32>::max());

    for (auto it = first; it != last; it++)
    {
        for (int i = 0; i < countOfMins; i++)
        {
            if (*it < vec[i])
            {
                for (int j = countOfMins - 1; j >= i; j--)
                {
                    if (j == i)
                    {
                        vec[j] = *it;
                        minIndexes[j] = it - first;
                        break;
                    }
                    else
                    {
                        vec[j] = vec[j - 1];
                        minIndexes[j] = minIndexes[j - 1];
                    }
                }
                break;
            }
        }
    }
    return minIndexes;
}


template <class InputIterator, typename Functor>
QVector <qint32> firstMinElements (InputIterator first, InputIterator last, qint32 countOfMins, Functor functor)
{
    QVector <float> vec(countOfMins, std::numeric_limits <decltype(functor(*first))>::max());
    QVector <qint32> minIndexes(countOfMins, std::numeric_limits <qint32>::max());

    for (auto it = first; it != last; it++)
    {
        for (int i = 0; i < countOfMins; i++)
        {
            if (functor(*it) < vec[i])
            {
                for (int j = countOfMins - 1; j >= i; j--)
                {
                    if (j == i)
                    {
                        vec[j] = functor(*it);
                        minIndexes[j] = it - first;
                        break;
                    }
                    else
                    {
                        vec[j] = vec[j - 1];
                        minIndexes[j] = minIndexes[j - 1];
                    }
                }
                break;
            }
        }
    }
    return minIndexes;
}


template <class CatalogRow>
void calculateStarCoordOffset(CatalogRow& starData, double epochFrom, double epochTo)
{
    if (epochFrom >= epochTo)
    {
        throw std::logic_error("Исходная эпоха больше или равна заданной для перевода");
    }
    double diff = epochTo - epochFrom;
    starData.alpha = starData.alpha + (starData.ualpha * diff) * arcSecToDegree;
    starData.delta = starData.delta + (starData.udelta * diff) * arcSecToDegree;
}


//template<class T, T v>
//struct ttrue_false {
//    static constexpr T value = v;
//};

//template<>
//struct ttrue_false<bool,true> {
//    static constexpr bool value = true;
//};
//typedef ttrue_false<bool,true> true_typep;

//template<>
//struct ttrue_false<bool,false> {
//    static constexpr bool value = false;
//};
//typedef ttrue_false<bool,false> false_typep;


//template <typename T, typename U>
//struct is_integral  : false_typep {};

//template <>
//struct is_integral<int,int>  : true_typep {};

//template <typename T, typename U>
//struct is_same : false_typep{};

//template <typename T>
//struct is_same<T,T> : true_typep{};

//template<class T ,
//         class = typename std::enable_if<std::is_integral<T>::value>::type >
//T foo3(T t) // обратите внимание, сигнатура функции не меняется
//{
//    return t;
//}


}

#endif // MATHFUNC_H
