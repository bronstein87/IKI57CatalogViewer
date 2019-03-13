#include <mathfunc.h>

namespace BOKZMath {


void calculateSunDirection(double JD_DEV, double *pSunI) noexcept
{
    double Ts, Ls, Ms, eS, Es, Vs, Qs, Ekl, AlphaS, DeltaS, QsVid, Om;

    Ts = JD_DEV+1;
    Ls = 279.69668+36000.76892*Ts+0.0003025*Ts*Ts;
    Ls /= 360.;
    Ls -= (int)Ls;
    Ls *= 360.;

    Ms = 358.47583+35999.04975*Ts-0.000150*Ts*Ts-0.0000033*Ts*Ts*Ts;
    Ms /= 360.;
    Ms -= (int)Ms;
    Ms *= 360.;

    Ms *= M_PI/180.;

    eS = 0.01675104-0.0000418*Ts-0.000000126 * Ts * Ts;
    Es = Ms;
    for (int i = 0; i < 10; i++) Es = Ms+eS*sin(Es);
    Vs=2*atan(sqrt((1+eS)/(1-eS))*tan(Es/2));
    Qs=Ls*M_PI/180.+Vs-Ms;

    Om = 259.18-1934.142*Ts;
    Om /= 360.;
    Om -= (int)Om;
    Om *= 2*M_PI;

    QsVid = Qs*180/M_PI-0.00569-0.00479*sin(Om);
    QsVid = QsVid*M_PI/180.;

    Ekl = 23.452294-0.0130125*Ts-0.00000164*Ts*Ts+0.000000503*Ts*Ts*Ts;
    Ekl += 0.00256*cos(Om);
    Ekl *= M_PI/180.;

    AlphaS = atan2(cos(Ekl)*sin(QsVid),cos(QsVid));
    DeltaS = asin(sin(Ekl)*sin(QsVid));

    if (AlphaS < 0) AlphaS += 2*M_PI;

    pSunI[0] = cos(AlphaS)*cos(DeltaS);
    pSunI[1] = sin(AlphaS)*cos(DeltaS);
    pSunI[2] = sin(DeltaS);

}

void calculateMoonDirection(double JD_DEV, double *pMoonI) noexcept
{
    double Ts, L1, Ms, M1, D, F, Om, e, Lam, B, Ekl, AlphaM, DeltaM;

    Ts=JD_DEV+1;
    //знак при Ts^2 должен быть минус, а не плюс
    L1=270.434164+481267.8831*Ts-0.001133*Ts*Ts+0.0000019*Ts*Ts*Ts;
    L1/=360.; L1-=(int)L1; L1*=360.;

    Ms=358.47583+35999.04975*Ts-0.000150*Ts*Ts-0.0000033*Ts*Ts*Ts;
    Ms/=360.; Ms-=(int)Ms; Ms*=2*M_PI;

    M1=296.104608+477198.8492*Ts+0.009192*Ts*Ts+0.0000144*Ts*Ts*Ts;
    M1/=360.; M1-=(int)M1; M1*=2*M_PI;

    D=350.737486+445267.1142*Ts-0.001436*Ts*Ts+0.0000019*Ts*Ts*Ts;
    D/=360.; D-=(int)D; D*=2*M_PI;

    F=11.250889+483202.0251*Ts-0.003211*Ts*Ts-0.000003*Ts*Ts*Ts;
    F/=360.; F-=(int)F; F*=2*M_PI;

    Om=259.183275-1934.1420*Ts+0.002078*Ts*Ts+0.0000022*Ts*Ts*Ts;
    Om/=360.; Om-=(int)Om; Om*=2*M_PI;

    e=1-0.002495*Ts-0.00000752*Ts*Ts;

    Lam=L1+6.288750*sin(M1)+1.274018*sin(2*D-M1)+0.658309*sin(2*D)
            +0.213616*sin(2*M1)-e*0.185596*sin(Ms)-0.114336*sin(2*F)
            +0.058793*sin(2*D-2*M1)+e*0.057212*sin(2*D-Ms-M1)+0.053320*sin(2*D+M1)
            +e*0.045874*sin(2*D-Ms)+e*0.041024*sin(M1-Ms)-0.034718*sin(D)-e*0.030465*sin(Ms+M1)
            +0.015326*sin(2*D-2*F)-0.012528*sin(2*F+M1)-0.01098*sin(2*F-M1)+0.010674*sin(4*D-M1)+0.010034*sin(3*M1);

    Lam/=360; Lam-=(int)Lam; Lam*=2*M_PI;

    B=5.128189*sin(F)+0.280606*sin(M1+F)+0.277693*sin(M1-F)+0.173238*sin(2*D-F)+0.055413*sin(2*D+F-M1)+0.046272*sin(2*D-F-M1)
            +0.032573*sin(2*D+F)+0.017198*sin(2*M1+F)+0.009267*sin(2*D+M1-F)+0.008823*sin(2*M1-F);
    B/=360; B-=(int)B; B*=2*M_PI;

    Ekl=23.452294-0.0130125*Ts-0.00000164*Ts*Ts+0.000000503*Ts*Ts*Ts;
    Ekl*=M_PI/180.;

    AlphaM=atan2((sin(Lam)*cos(Ekl)-tan(B)*sin(Ekl)),cos(Lam));
    DeltaM= asin(sin(B)*cos(Ekl)+cos(B)*sin(Ekl)*sin(Lam));

    pMoonI[0] = cos(AlphaM) * cos(DeltaM);
    pMoonI[1] = sin(AlphaM) * cos(DeltaM);
    pMoonI[2] = sin(DeltaM);
}




void multMatrixVector(const double M[3][3], const double V[3], double VR[3]) noexcept
{
    VR[0] = M[0][0]*V[0]+M[0][1]*V[1]+M[0][2]*V[2];
    VR[1] = M[1][0]*V[0]+M[1][1]*V[1]+M[1][2]*V[2];
    VR[2] = M[2][0]*V[0]+M[2][1]*V[1]+M[2][2]*V[2];
}


/*меняем местами 1,2 и 3,4 байты qint32 местами*/
// fix
void swapShort(void* value) noexcept
{
    quint16 parts2Bytes[2];
    memcpy(parts2Bytes, value, sizeof(short));
    memcpy(parts2Bytes + 1, (short*)(value) + 1, sizeof(short));
    memcpy((short*)(value) + 1,parts2Bytes, sizeof(short));
    memcpy(value, parts2Bytes + 1, sizeof(short));
}




/*корректируем весь массив локализованных объектов*/
void correctLocArray(float locArray[16][4]) noexcept
{
    quint16 locColumns = 16;
    quint16 locRows = 4;

    /* конвертируем Loc в формат postgres массива (хранить будем его как одномерный, иначе долго)*/
    for(qint32 locColumn = 0;locColumn < locColumns; locColumn++)
    {
        for(qint32 locRow = 0; locRow < locRows; locRow++)
        {
            swapShort(&locArray[locColumn][locRow]);
        }
    }
}


void correctAngularVArray(float Wo[3]) noexcept
{
    quint16 WoLenght = 3;
    for(qint32 i = 0;i < WoLenght;i ++)
    {
        swapShort(&Wo[i]);
    }

}


// расчёт юлианской даты
double calculateJulianDate(const QDateTime& dateTime) noexcept
{
    double julianDate =
            static_cast <double> (dateTime.date().toJulianDay()) - 0.5;
    julianDate += static_cast <double> (dateTime.time().hour()) / 24
            + static_cast <double> (dateTime.time().minute()) / 1440
            + static_cast <double> (dateTime.time().second()) / 86400;
    return julianDate;
}


double calculateJulianCenturies(const QDateTime& dateTime) noexcept
{
    double jd = calculateJulianDate(dateTime);
    return (jd - BOKZMath::julianZeroDate) / BOKZMath::countOfDaysInCentury;
}


inline double mSecsToSecs(quint64 mSecsSinceEpoch) noexcept
{
    return static_cast <double> (mSecsSinceEpoch / 1000);
}


// для вычисления дат времени привязки (юлианская дата на полночь)
double calculateDatePr(const QDateTime& dateTime, int timePr) noexcept
{
    qint64 julianDate  = dateTime.addSecs(-timePr).date().toJulianDay();
    return static_cast <double> (julianDate) - 0.5;
}


// вычисляет дату как TDateTime::value, УЧТЕНЫ миллисек., но не проверено.
double toTDateTimeValue(const QDateTime& datetime) noexcept
{
    auto daysToTDateTimeValue = std::abs(datetime.daysTo(QDateTime(QDate(1899, 12, 30))));
    double partOfDay = static_cast <double> (abs(datetime.time().msecsTo(QTime(0,0,0)))) / mSecsInDay;
    return static_cast <double> (daysToTDateTimeValue) + partOfDay;
}

QDateTime fromTDateTime(double value) noexcept
{
    return QDateTime(QDate(1899, 12, 30))
            .addDays(static_cast <qint32> (value))
            .addMSecs(round((value - floor(value)) * mSecsInDay));
}


/*смотри http://law.rufox.ru/view/standarti/1672.htm*/
double starsTime2(double JD) noexcept
{
    double     l1;      // средняя аномалия Солнца
    double     F;       // средний аргумент широты Луны
    double     D;       // разность средних долгот Луны и Солнца
    double     W;       // средняя долгота восходящего узла орбиты Луны на эклиптике
    double     Nf;      // значения нутации в долготе
    double     Ne;      //значения нутации в наклоне
    double     e0;      // средний наклон эклиптики к экватору, выраженный в радианах
    double     M;       //всемирное время  рассматриваемой даты, выраженное в долях суток
    double     tau;     //интервал времени от эпохи T0  до эпохи t в юлианских столетиях по 36525 средних солнечных суток
    double     tau_2;   // тоже самое в квадрате
    double     tau_3;   // тоже самоее в кубе
    double     Ssr_rad; // гринвичское среднее звездное время


    tau = JD/36525.;
    tau_2 = tau*tau;
    tau_3 = tau_2*tau;

    M = JD+0.5-(qint32)JD;
    Ssr_rad = 1.7533685592+0.0172027918051*JD+6.2831853072*M+6.7707139e-6*tau_2
            -4.50876e-10*tau_2*tau;

    l1 = 6.24003594 + 628.30195602 *tau - 2.7974e-6 *tau_2-5.82e-8*tau_3;
    F = 1.62790193 + 8433.46615831 *tau - 6.42717e-5*tau_2 + 5.33e-8*tau_3;
    D = 5.19846951 + 7771.37714617 *tau - 3.34085e-5*tau_2 + 9.21e-8*tau_3;
    W = 2.182438624 - 33.757045936*tau + 3.61429e-5*tau_2 + 3.88e-8*tau_3;
    Nf = -0.83386e-4*sin(W)+0.9997e-6*sin(2*W)-0.63932e-5*sin(2*(F-D+W))+
            0.6913e-6*sin(l1)-0.11024e-5*sin(2*(F+W));
    Ne = 0.44615e-4*cos(W)+0.27809e-5*cos(2*(F-D+W))+0.474e-6*cos(2*(F+W));
    e0 = 0.4090928042-0.2269655e-3*tau-0.29e-8*tau_2+0.88e-8*tau_3;

    //учитываем нутацию в прямом восхождении
    Ssr_rad += Nf*cos(e0+Ne);

    //приводим звездное время к "нормальному" виду
    while (Ssr_rad > 2*M_PI) Ssr_rad = Ssr_rad-2*(M_PI);

    return Ssr_rad;
}


void getGSKISKMatrix(double JD, double MG[3][3]) noexcept
{
    double ST = starsTime2(JD); // истинное звездное время

    MG[0][0] = cos(ST);
    MG[0][1] = -sin(ST);
    MG[0][2] = 0;

    MG[1][0] = sin(ST);
    MG[1][1] = cos(ST);
    MG[1][2] = 0;

    MG[2][0] = 0;
    MG[2][1] = 0;
    MG[2][2] = 1;
}


void  transSpeedGSKtoISK(double JD, double CG[3], double VG[3], double VI[3]) noexcept
{
    double MG[3][3]; // матрица перехода от ГСК к ИСК
    getGSKISKMatrix(JD, MG);

    constexpr const double W = 15.041 / radToSec;

    double CI [3];
    multMatrixVector(MG, CG, CI);
    multMatrixVector(MG, VG, VI);

    for (qint32 i = 0; i < 3; i ++)
    {
        CI[i] /= 1000.; // в километры
        VI[i] /= 1000.;
    }

    VI[0] -= W * CI[1];  VI[1] += W * CI[0];
}

void  transCoordsGSKtoISK(double JD, double CG[3], double CI[3]) noexcept
{

    double MG[3][3]; // матрица перехода от ГСК к ИСК
    getGSKISKMatrix(JD,MG);
    multMatrixVector(MG, CG, CI);

    for (qint32 i = 0; i < 3; i++)
    {
        CI[i] /= 1000.; // в километры
    }

}


void quatToMatr(const double Quat[], double mOrnt[3][3]) noexcept
{

    mOrnt[0][0] = Quat[0]*Quat[0]+Quat[1]*Quat[1]-Quat[2]*Quat[2]-Quat[3]*Quat[3];
    mOrnt[0][1] = 2.0*(Quat[1]*Quat[2]+Quat[0]*Quat[3]);
    mOrnt[0][2] = 2.0*(Quat[1]*Quat[3]-Quat[0]*Quat[2]);
    mOrnt[1][0] = 2.0*(Quat[1]*Quat[2]-Quat[0]*Quat[3]);
    mOrnt[1][1] = Quat[0]*Quat[0]-Quat[1]*Quat[1]+Quat[2]*Quat[2]-Quat[3]*Quat[3];
    mOrnt[1][2] = 2.0*(Quat[2]*Quat[3]+Quat[0]*Quat[1]);
    mOrnt[2][0] = 2.0*(Quat[1]*Quat[3]+Quat[0]*Quat[2]);
    mOrnt[2][1] = 2.0*(Quat[2]*Quat[3]-Quat[0]*Quat[1]);
    mOrnt[2][2] = Quat[0]*Quat[0]-Quat[1]*Quat[1]-Quat[2]*Quat[2]+Quat[3]*Quat[3];
}


double atan2m(double yf, double xf) noexcept
{
    double ang;
    if (fabs(xf) > 1e-10)
    {
        ang = atan2(yf, xf);
    }
    else
    {
        if (yf > 0)
        {
            ang = M_PI/2.;
        }
        else
        {
            ang = -M_PI/2.;
        }
    }
    return ang;
}

void matrToEkvA(double mOrnt[3][3], double& al, double& dl, double& Az, AxisType axis) noexcept
{
    qint32 row = static_cast <qint32> (axis);
    dl = asin(mOrnt[row][2]);
    al = atan2m(mOrnt[row][1], mOrnt[row][0]);
    if (al < 0)
    {
        al += 2 * M_PI;
    }
    if (axis == AxisType::zAxis)
    {
        Az = atan2m(mOrnt[0][2], mOrnt[1][2]);
        if (Az < 0)
        {
            Az += 2 * M_PI;
        }
    }
    else
    {
        Az = 0; // temporary
    }

}

void quatToEkvA(const double Quat[], double EkvA[3], AxisType axis) noexcept
{
    double Matr[3][3], Al, Dl, Az;
    quatToMatr(Quat, Matr);
    matrToEkvA(Matr, Al, Dl, Az, axis);

    EkvA[0] = Al * radToDegrees;
    EkvA[1] = Dl * radToDegrees;
    EkvA[2] = Az * radToDegrees;
}


double acosm(double xf) noexcept
{
    if (xf>1.) xf = 1.;
    else if (xf<-1.) xf -= 1.;
    return acos(xf);
}

double asinm(double x) noexcept
{
    if (x > 1.) x = 1.;
    else if (x < -1.) x = -1.;

    return asin(x);
}

void multiplyMatrix(const double Matr1[3][3],const double Matr2[3][3], double Matr[3][3]) noexcept
{
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            double buf = 0;
            for (int k = 0; k < 3; k++)
            {
                buf = buf + Matr1[i][k] * Matr2[k][j];
            }
            Matr[i][j] = buf;
        }
    }
}

void getAngularDisplacementFromOrientMatr(const double mOrnt_pr[3][3],const double mOrnt[3][3], double Wop [3]) noexcept

{
    double MT_ornt_pr[3][3], dMB[3][3];
    double delta, sdt;
    // считаем матрицу, дающую противоположный поворот
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            MT_ornt_pr[i][j] = mOrnt_pr[j][i];
        }
    }
    // перемножаем, чтобы получить разницу
    multiplyMatrix(mOrnt, MT_ornt_pr, dMB);
    delta=(dMB[0][0] + dMB[1][1] + dMB[2][2] - 1.) / 2.;
    // если угол очень маленький, то считаем дельта равной 0
    if (delta > 1) delta = 1;

    if (fabs(delta - 1.0) < 1E-20)
    {
        Wop[0] = 0;
        Wop[1] = 0;
        Wop[2] = 0;
    }
    else
    {
        delta = acosm(delta);
        sdt = sin(delta);
        Wop[0]= -delta * (dMB[2][1] - dMB[1][2]) / (2.0 * sdt);
        Wop[1]= -delta * (dMB[0][2] - dMB[2][0]) / (2.0 * sdt);
        Wop[2]= -delta * (dMB[1][0] - dMB[0][1]) / (2.0 * sdt);
    }
}

double calculateAngleAxis(const double quat1[], const double quat2[], AxisType axis) noexcept
{
    double orientMatrix1[3][3];
    BOKZMath::quatToMatr(quat1, orientMatrix1);

    /*считаем углы между заданной осью*/
    qint32 axisBetween = static_cast <qint32> (axis);

    double lfirst = orientMatrix1[axisBetween][0];
    double mfirst = orientMatrix1[axisBetween][1];
    double nfirst = orientMatrix1[axisBetween][2];

    double orientMatrix2[3][3];
    BOKZMath::quatToMatr(quat2, orientMatrix2);

    double lsecond = orientMatrix2[axisBetween][0];
    double msecond = orientMatrix2[axisBetween][1];
    double nsecond = orientMatrix2[axisBetween][2];

    return std::acos(calculateScalarProduct(lfirst, lsecond, mfirst, msecond, nfirst, nsecond)) * BOKZMath::radToDegrees;
}


QVector <QPointF> createHistogramm(size_t histSize, float shiftRange, QVector <float> firstX, QVector <float> firstY, QVector <float> secondX, QVector <float> secondY) noexcept
{
    QVector <QVector <qint32>> histogramm;
    histogramm.resize(histSize);
    for(auto& i : histogramm)
    {
        i.resize(histSize);
    }
    qint32 centerX, centerY, range;
    centerX = centerY = (histSize / 2) + 1;
    range = histSize / 2;

    qint32 countObjectsFirst = firstX.size();
    qint32 countObjectsSecond = secondX.size();

    qint32 dx, dy;
    for(qint32 i = 0; i < countObjectsFirst; i++)
    {
        if (qFuzzyCompare(firstX[i], 0) && qFuzzyCompare(firstY[i], 0))
        {
            break;
        }
        for (qint32 j = 0; j < countObjectsSecond; j++)
        {
            dx = round(firstX[i] - secondX[j]);
            dy = round(firstY[i] - secondY[j]);

            if (abs(dx) < range && abs(dy) < range)
            {
                ++histogramm[centerX + dx][centerY + dy];
            }
        }
    }
    auto maxHist = matrixMax(histogramm);
    qint32 shiftX = maxHist.first - centerX;
    qint32 shiftY = maxHist.second - centerY;

    QVector <QPointF> shifts;
    float currShiftX = 0.0;
    float currShiftY = 0.0;

    // считаем смещения для всех совпадающих по порядку объектов
    for(qint32 i = 0; i < countObjectsFirst; i++)
    {
        currShiftX = firstX[i] - secondX[i];
        currShiftY = firstY[i] - secondY[i];

        if (currShiftX <= (shiftX + shiftRange) && currShiftX >= (shiftX - shiftRange)
                && currShiftY <= (shiftY + shiftRange) && currShiftY >= (shiftY - shiftRange))
        {
            shifts.append(QPointF(currShiftX, currShiftY));
        }
        else
        {
            shifts.append(QPointF(0.0,0.0));
        }
    }

    // считаем смещения для тех объектов, которые могут находиться в разных порядках
    // а возможно он уже просто пропал, тогда ничего не изменится
    for(qint32 i = 0; i < shifts.size(); i++)
    {
        if (qFuzzyCompare(shifts[i].x(), 0) && qFuzzyCompare(shifts[i].y(), 0))
        {
            for(qint32 j = 0; j < countObjectsSecond; j++)
            {
                currShiftX = firstX[i] - secondX[j];
                currShiftY = firstY[i] - secondY[j];

                if (currShiftX <= (shiftX + shiftRange) && currShiftX >= (shiftX - shiftRange)
                        && currShiftY <= (shiftY + shiftRange) && currShiftY >= (shiftY - shiftRange))
                {
                    shifts[i] = QPointF(currShiftX, currShiftY);
                }
            }
        }
    }
    return shifts;
}

// потом отрефакторить
QVector<QPointF> createHistogramm(size_t histSize, float shiftRange, const QVector<QPointF>& firstCoords, const QVector<QPointF>& secondCoords) noexcept
{
    QVector <QVector <qint32>> histogramm;
    histogramm.resize(histSize);
    for(auto& i : histogramm)
    {
        i.resize(histSize);
    }
    qint32 centerX, centerY, range;
    centerX = centerY = (histSize / 2) + 1;
    range = histSize / 2;

    qint32 countObjectsFirst = firstCoords.size();
    qint32 countObjectsSecond = secondCoords.size();
    qint32 dx, dy;
    for(qint32 i = 0; i < countObjectsFirst; i ++)
    {
        if (qFuzzyCompare(firstCoords[i].x(), 0) && qFuzzyCompare(firstCoords[i].y(), 0))
        {
            break;
        }
        for (qint32 j = 0; j < countObjectsSecond; j ++)
        {
            dx = round(firstCoords[i].x() - secondCoords[j].x());
            dy = round(firstCoords[i].y() - secondCoords[j].y());

            if (abs(dx) < range && abs(dy) < range)
            {
                ++histogramm[centerX + dx][centerY + dy];
            }
        }
    }
    auto maxHist = matrixMax(histogramm);
    qint32 shiftX = maxHist.first - centerX;
    qint32 shiftY = maxHist.second - centerY;

    QVector <QPointF> shifts;
    float currShiftX = 0.0;
    float currShiftY = 0.0;

    // считаем смещения для всех совпадающих по порядку объектов
    for(qint32 i = 0; i < countObjectsFirst; i ++)
    {
        currShiftX = firstCoords[i].x() - secondCoords[i].x();
        currShiftY = firstCoords[i].y() - secondCoords[i].y();

        if (currShiftX <= (shiftX + shiftRange) && currShiftX >= (shiftX - shiftRange)
                && currShiftY <= (shiftY + shiftRange) && currShiftY >= (shiftY - shiftRange))
        {
            shifts.append(QPointF(currShiftX, currShiftY));
        }
        else
        {
            shifts.append(QPointF(0.0,0.0));
        }
    }
    // считаем смещения для тех объектов, которые могут находиться в разных порядках
    // а возможно он уже просто пропал, тогда ничего не изменится
    for(int i = 0; i < shifts.size(); i ++)
    {
        if (qFuzzyCompare(shifts[i].x(), 0) && qFuzzyCompare(shifts[i].y(), 0))
        {
            for(qint32 j = 0; j < countObjectsSecond; j++)
            {
                currShiftX = firstCoords[i].x() - secondCoords[j].x();
                currShiftY = firstCoords[i].y() - secondCoords[j].y();

                if (currShiftX <= (shiftX + shiftRange) && currShiftX >= (shiftX - shiftRange)
                        && currShiftY <= (shiftY + shiftRange) && currShiftY >= (shiftY - shiftRange))
                {
                    shifts[i] = QPointF(currShiftX, currShiftY);
                }
            }
        }
    }
    return shifts;
}

qint64 roundUp(qint64 numToRound, qint64 multiple) noexcept
{
    if (multiple == 0)
        return numToRound;

    auto remainder = std::remainder(numToRound, multiple);
    if (qFuzzyCompare(remainder, 0))
        return numToRound;

    if (numToRound < 0)
        return -(std::abs(numToRound) - remainder);
    else
        return numToRound + multiple - remainder;
}

QDateTime timePrToDateTime(const double timePr, const QDateTime& timePrStartData) noexcept
{
    qint64 dtInMsecs = timePr * mSecsInSec;
    return timePrStartData.addMSecs(dtInMsecs);
}

QDateTime timePrToDateTime(const qint32 timePr, const QDateTime& timePrStartData) noexcept
{
    qint64 dtInMsecs = static_cast<qint64> (timePr) * mSecsInSec;
    return timePrStartData.addMSecs(dtInMsecs);
}

QDateTime timePrToDateTime(const quint64 timePr, const QDateTime& timePrStartData) noexcept
{

    return timePrStartData.addMSecs(timePr * mSecsInSec);
}

QVector< QVector<double> > calcTransitionMatrix(double pointAlpha, double pointBeta, double pointAzimut) noexcept
{

    QVector< QVector<double> > trMat(3);

    for (int i = 0;i < trMat.size(); i++)
    {

        trMat[i].resize(3);
    }
    double PS,PC,QS,QC,RS,RC;
    PS = sin(pointAzimut * degreesToRad); PC = cos(pointAzimut * degreesToRad);
    QS = sin(pointBeta * degreesToRad); QC = cos(pointBeta * degreesToRad);
    RS = sin(pointAlpha * degreesToRad); RC = cos(pointAlpha * degreesToRad);
    trMat[0][0] = -PC * RS - PS * RC * QS;
    trMat[0][1] = PC * RC - PS * RS * QS;
    trMat[0][2] = PS * QC;
    trMat[1][0] = PS * RS - PC * RC * QS;
    trMat[1][1] = -PS * RC - PC * RS * QS;
    trMat[1][2] = PC * QC;
    trMat[2][0] = QC * RC;
    trMat[2][1] = QC * RS;
    trMat[2][2] = QS;

    return trMat;

}


void calcTransitionMatrix(double pointAlpha, double pointBeta, double pointAzimut, double mOrnt[3][3]) noexcept
{
    double PS,PC,QS,QC,RS,RC;

    PS = sin(pointAzimut * degreesToRad); PC= cos(pointAzimut * degreesToRad);
    QS = sin(pointBeta * degreesToRad); QC = cos(pointBeta * degreesToRad);
    RS = sin(pointAlpha * degreesToRad); RC = cos(pointAlpha * degreesToRad);
    mOrnt[0][0] = -PC * RS - PS * RC * QS;
    mOrnt[0][1] = PC * RC - PS * RS * QS;
    mOrnt[0][2] = PS * QC;
    mOrnt[1][0] = PS * RS - PC * RC * QS;
    mOrnt[1][1] = -PS * RC - PC * RS * QS;
    mOrnt[1][2] = PC * QC;
    mOrnt[2][0] = QC * RC;
    mOrnt[2][1] = QC * RS;
    mOrnt[2][2] = QS;
}


void calculateDirectionVector(double alpha, double delta, double& l, double& m, double& n)
{
    l = cos(delta * degreesToRad) * cos(alpha * degreesToRad);
    m = cos(delta * degreesToRad) * sin(alpha * degreesToRad);
    n = sin(delta * degreesToRad);
}

double calculateScalarProduct(double lOz, double lSt, double mOz, double mSt, double nOz, double nSt) noexcept
{
    double scalarProduct = lOz * lSt + mOz * mSt + nOz * nSt;

    if (scalarProduct > 1.0)// проверка на выход значения косинуса за диапазон [-1,1]
    {
        scalarProduct = 1;
    }
    else if (scalarProduct < -1.0)
    {
        scalarProduct = -1;
    }
    return scalarProduct;
}

double calculateScalarProductAngles(double anglesFirst[3], double anglesSecond[3])
{
    return acosm(sin(anglesFirst[1] * degreesToRad) * sin(anglesSecond[1] * degreesToRad)
            + cos(anglesFirst[1] * degreesToRad) * cos(anglesSecond[1] * degreesToRad)
             * cos((anglesFirst[0] - anglesSecond[0]) * degreesToRad)) * BOKZMath::radToDegrees;
}

void calculateAlphaDelta(double l, double m, double n, double& alpha, double& delta) noexcept
{
    delta = asin(n) * radToDegrees;
    alpha = atan2m(m, l);
    if (alpha < 0)
    {
        alpha += 2 * M_PI;
    }
    alpha *= radToDegrees;
}

QVector <qint32> firstMinDistanceTable(RecognizedInfo** distMatrix, qint32 countOfMins, qint32 objectIndex, qint32 size)
{
    QVector <float> vec(countOfMins, std::numeric_limits <float>::max());
    QVector <qint32> minIndexes(countOfMins, std::numeric_limits <qint32>::max());

    for (qint32 i = 0; i < size; i++)
    {
        if (i == objectIndex) continue;

        float dist = 0;
        if (objectIndex > i)
            dist = distMatrix[objectIndex][i].distance;
        else dist = distMatrix[i][objectIndex].distance;

        for (qint32 j = 0; j < countOfMins; j++)
        {
            if (dist < vec[j])
            {
                for (qint32 k = countOfMins - 1; k >= j; k--)
                {
                    if (k == j)
                    {
                        vec[k] = dist;
                        minIndexes[k] = i;
                        break;
                    }
                    else
                    {
                        vec[k] = vec[k - 1];
                        minIndexes[k] = minIndexes[k - 1];
                    }
                }
                break;
            }
        }
    }
    return minIndexes;
}
void calculateLMNImage(double x, double y, double focus, double lmn[3]) noexcept
{
    double length = sqrt(x * x + y * y + focus * focus);
    lmn[0] = - x / length;
    lmn[1] = - y / length;
    lmn[2] = focus / length;
}

double sqrtm(double x)
{
    if (x < 1e-20)
        return 0.;
    else
        return sqrt(x);
}

int LUPDecompose(double **A, int N, double Tol, int *P) noexcept
{

    int i, j, k, imax;
    double maxA, *ptr, absA;

    for (i = 0; i <= N; i++)
        P[i] = i; //Unit permutation matrix, P[N] initialized with N

    for (i = 0; i < N; i++) {
        maxA = 0.0;
        imax = i;

        for (k = i; k < N; k++)
            if ((absA = fabs(A[k][i])) > maxA) {
                maxA = absA;
                imax = k;
            }

        if (maxA < Tol) return 0; //failure, matrix is degenerate

        if (imax != i) {
            //pivoting P
            j = P[i];
            P[i] = P[imax];
            P[imax] = j;

            //pivoting rows of A
            ptr = A[i];
            A[i] = A[imax];
            A[imax] = ptr;

            //counting pivots starting from N (for determinant)
            P[N]++;
        }

        for (j = i + 1; j < N; j++) {
            A[j][i] /= A[i][i];

            for (k = i + 1; k < N; k++)
                A[j][k] -= A[j][i] * A[i][k];
        }
    }

    return 1;  //decomposition done
}

double LUPDeterminant(double **A, int *P, int N) noexcept
{

    double det = A[0][0];

    for (int i = 1; i < N; i++)
        det *= A[i][i];

    if ((P[N] - N) % 2 == 0)
        return det;
    else
        return -det;
}

void LUPInvert(double** A, int* P, int N, double** IA) noexcept
{
    for (int j = 0; j < N; j++) {
        for (int i = 0; i < N; i++) {
            if (P[i] == j)
                IA[i][j] = 1.0;
            else
                IA[i][j] = 0.0;

            for (int k = 0; k < i; k++)
                IA[i][j] -= A[i][k] * IA[k][j];
        }

        for (int i = N - 1; i >= 0; i--) {
            for (int k = i + 1; k < N; k++)
                IA[i][j] -= A[i][k] * IA[k][j];

            IA[i][j] = IA[i][j] / A[i][i];
        }
    }
}

double getDet(int nElem, double** DRVM)
{
    double **tmpM = new double* [nElem];
    for (int count = 0; count < nElem; count++)
    {
        tmpM[count] = new double [nElem];
    }
    for (int i = 0; i < nElem; i++)
    {
        for (int j = 0; j < nElem; j++)
        {
            tmpM[i][j] = DRVM[i][j];
        }
    }
    int* P = new int [nElem + 1];
    int status = LUPDecompose(tmpM, nElem, 1e-10, P);
    if (!status) throw std::logic_error("ERROR LUPDecompose");
    double DET = LUPDeterminant(tmpM, P, nElem);
    for (int j = 0; j < nElem; j++)
    {
        delete [] tmpM[j];
    }
    delete [] tmpM;

    return DET;
}


void invertMatrix(double** DRVM, double** DRVM_1, int nElem)
{
    double** tmpM = new double* [nElem];
    for (int count = 0; count < nElem; count++)
    {
        tmpM[count] = new double [nElem];
    }
    for (int i = 0; i < nElem; i++)
    {
        for (int j = 0; j < nElem; j++)
        {
            tmpM[i][j] = DRVM[i][j];
        }
    }
    int* P = new int [nElem + 1];
    int status = LUPDecompose(tmpM, nElem, 1e-10, P);
    if (!status) throw std::logic_error("ERROR LUPDecompose");
    LUPInvert(tmpM, P, nElem, DRVM_1);
}

int minorM(int z, int x, int N, double** A,double** C) noexcept
{
    int i, h, j, k;
    for(h = 0,i = 0; i < N - 1 ; i++, h++)
    {
        if (i == z)
            h++;
        for (k = 0,j = 0;j < N - 1; j++, k++)
        {
            if (k == x)
                k++;
            C[i][j] = A[h][k];
        }
    }
    return 0;
}

double calculateDistorsio(double coordC, double coordA, double coordB, const QList<double>& distorsioCoef) noexcept
{
    return coordC - calculateDistorsioDelta(coordA, coordB, distorsioCoef);
}

double calculateDistorsioDelta(double coordA, double coordB, const QList<double>& distorsioCoef) noexcept
{

    double delta = distorsioCoef[0] + distorsioCoef[1] * coordA + distorsioCoef[2] * coordB
            +distorsioCoef[3] * pow(coordA, 2) + distorsioCoef[4] * coordA * coordB
            +distorsioCoef[5] * pow(coordB, 2)+ distorsioCoef[6] * pow(coordA, 3)
            +distorsioCoef[7] * pow(coordA, 2) * coordB + distorsioCoef[8] * coordA * pow(coordB, 2)
            +distorsioCoef[9] * pow(coordB, 3) + distorsioCoef[10] * pow(coordA, 4)
            +distorsioCoef[11] * pow(coordA, 3) * coordB + distorsioCoef[12] * pow(coordB, 2) * pow(coordA, 2)
            +distorsioCoef[13] * coordA * pow(coordB, 3) + distorsioCoef[14] * pow(coordB, 4)
            +distorsioCoef[15] * pow(coordA, 5) + distorsioCoef[16] * pow(coordA, 4) * coordB
            +distorsioCoef[17] * pow(coordA, 3) * pow(coordB, 2)
            +distorsioCoef[18] * pow(coordA, 2) * pow(coordB, 3)
            +distorsioCoef[19] * coordA * pow(coordB,4)
            +distorsioCoef[20] * pow(coordB, 5);
    // +
        //            +distorsioCoef[21]*x_6+distorsioCoef[22]*x_5*y[i]+distorsioCoef[23]*x_4*y_2+distorsioCoef[24]*x_3*y_3+distorsioCoef[25]*x_2*y_4+distorsioCoef[26]*x[i]*y_5+distorsioCoef[27]*y_6
        //            +distorsioCoef[28]*x_7+distorsioCoef[29]*x_6*y[i]+distorsioCoef[30]*x_5*y_2+distorsioCoef[31]*x_4*y_3+distorsioCoef[32]*x_3*y_4+distorsioCoef[33]*x_2*y_5+distorsioCoef[34]*x[i]*y_6+distorsioCoef[35]*y_7
        //            +distorsioCoef[36]*x_8+distorsioCoef[37]*x_7*y[i]+distorsioCoef[38]*x_6*y_2+distorsioCoef[39]*x_5*y_3+distorsioCoef[40]*x_4*y_4+distorsioCoef[41]*x_3*y_5+distorsioCoef[42]*x_2*y_6+distorsioCoef[43]*x[i]*y_7+distorsioCoef[44]*y_8
        //            +distorsioCoef[45]*x_9+distorsioCoef[46]*x_8*y[i]+distorsioCoef[47]*x_7*y_2+distorsioCoef[48]*x_6*y_3+distorsioCoef[49]*x_5*y_4+distorsioCoef[50]*x_4*y_5+distorsioCoef[51]*x_3*y_6+distorsioCoef[52]*x_2*y_7+distorsioCoef[53]*x[i]*y_8+distorsioCoef[54]*y_9;;


    return delta;
}


bool checkQuatNorm(double q[4]) noexcept
{
    double len = q[0] * q[0] +
            q[1] * q[1] +
            q[2] * q[2] +
            q[3] * q[3];
    if (qAbs(len - 1.0) < 0.000001)
        return true;
    else
        return false;

}

void rotateOY(double alpha, double mIn[3][3], double mOut[3][3]) noexcept
{
    double m[3][3] = {{cos(alpha), 0, sin(alpha)},
                      {0, 1, 0},
                      {-sin(alpha), 0, cos(alpha)}};
    multiplyMatrix(m, mIn, mOut);

}

void rotateOZ(double alpha, double mIn[3][3], double mOut[3][3]) noexcept
{
    double m[3][3] = {{cos(alpha), -sin(alpha), 0},
                      {sin(alpha), cos(alpha), 0},
                      {0, 0, 1}};
    multiplyMatrix(m, mIn, mOut);
}

void rotateOX(double alpha, double mIn[3][3], double mOut[3][3]) noexcept
{
    double m[3][3] = {{1, 0, 0},
                      {0, cos(alpha), -sin(alpha)},
                      {0, sin(alpha), cos(alpha)}};
    multiplyMatrix(m, mIn , mOut);
}

}





