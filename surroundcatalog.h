#ifndef SURROUNDCATALOG_H
#define SURROUNDCATALOG_H
#include <QVector>
#include <QString>
#include <QDebug>
#include <QFile>
#include <QDataStream>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <mathfunc.h>


using namespace BOKZMath;

#pragma pack(push,1)
struct BaseStars
{
       int num;
       float alpha;
       float delta;
       float mv;
       float bv;
       unsigned char k1;
       unsigned char k2;
       short n1;
       int n2;
};
#pragma pack(pop)

#pragma pack(push,1)
struct SurroundStars
{
       float alpha;
       float delta;
       float mv;
};
#pragma pack(pop)

enum Stars
{
    HipNum,
    Alpha,
    Delta,
    Mv,
    Bv,
    Spec,
    ParentNum,
    Id,
    L,
    M,
    N
};



class SurroundCatalog
{
public:
    SurroundCatalog(const QString& path);

    QSqlDatabase getDb() const {return db;}

private:
    void readCatalog(const QString& path);

    void fillDatabase();

    QVector <BaseStars> baseStars;
    QVector <SurroundStars> surroundStars;
    QSqlDatabase db;
};

#endif // SURROUNDCATALOG_H
