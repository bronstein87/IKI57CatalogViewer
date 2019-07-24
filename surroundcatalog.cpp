#include "surroundcatalog.h"
#include <QSqlError>
#include <sqlite3.h>
#include <QSqlDriver>
SurroundCatalog::SurroundCatalog(const QString& path)
    : db (QSqlDatabase::addDatabase("QSQLITE"))
{
    db.setDatabaseName("sqlitedb/catdb.db");
    db.open();

    QSqlQuery isEmptyQuery("SELECT COUNT(*) FROM stars");

    isEmptyQuery.next();
    if (isEmptyQuery.value(0).toInt() == 0)
    {
        readCatalog(path);
        fillDatabase();
        fillIntegralBright();
    }
    QVariant v = db.driver()->handle();
    if (v.isValid() && qstrcmp(v.typeName(), "sqlite3*") == 0)
    {
        sqlite3_initialize();
        sqlite3 *db_handle = *static_cast<sqlite3 **>(v.data());
        if (db_handle != nullptr)
        {
            qDebug() << sqlite3_enable_load_extension(db_handle, 1);
            QSqlQuery query;
            query.exec("SELECT load_extension('libsqlitefunctions.so');");
            if (query.lastError() .isValid())
            {
                qDebug() << "Error: cannot load the libsqlitefunctions.so (" << query.lastError().text()<<")";
            }
        }
    }
}


void SurroundCatalog::readCatalog(const QString& path)
{
    QFile baseCat(path + "/OP10377.CAT");
    if (baseCat.open(QIODevice::ReadOnly))
    {
        QDataStream in (&baseCat);
        BaseStars tmp;
        while (in.readRawData((char*)&tmp, sizeof(tmp)))
        {
            baseStars.append(tmp);
        }
    }
    QFile surroundCat (path + "OKR10377.CAT");
    if (surroundCat.open(QIODevice::ReadOnly))
    {
        QDataStream in (&surroundCat);
        SurroundStars tmp;
        while (in.readRawData((char*)&tmp, sizeof (tmp)))
        {
            surroundStars.append(tmp);
        }
    }
}


void SurroundCatalog::fillDatabase()
{
    db.transaction();
    QSqlQuery query;
    query.prepare("INSERT INTO stars (hip_num, alpha, delta, mv, bv, spec, l, m, n, n1, n2) "
                  "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
    double l, m, n;
    for (const auto& i : baseStars)
    {
        double alpha = i.alpha * radToDegrees;
        double delta = i.delta * radToDegrees;
        calculateDirectionVector(alpha, delta, l, m, n);
        query.addBindValue(i.num);
        query.addBindValue(alpha);
        query.addBindValue(delta);
        query.addBindValue(i.mv);
        query.addBindValue(i.bv);
        query.addBindValue(QString("%1%2")
                           .arg(QChar(i.k1))
                           .arg(QChar(i.k2)));
        query.addBindValue(l);
        query.addBindValue(m);
        query.addBindValue(n);
        query.addBindValue(i.n1);
        query.addBindValue(i.n2);
        query.exec();
    }
    query.prepare("INSERT INTO star_ocr (id, alpha, delta, mv, l, m, n) "
                  "VALUES (?, ?, ?, ?, ?, ?, ?)");
    int counter = 0;
    for (const auto& i : surroundStars)
    {
        qint32 id = counter++;
        double alpha = i.alpha * radToDegrees;
        double delta = i.delta * radToDegrees;
        calculateDirectionVector(alpha, delta, l, m, n);
        query.addBindValue(id);
        query.addBindValue(alpha);
        query.addBindValue(delta);
        query.addBindValue(i.mv);
        query.addBindValue(l);
        query.addBindValue(m);
        query.addBindValue(n);
        query.exec();
    }
    qDebug() << "end";
    db.commit();

}
void SurroundCatalog::fillIntegralBright()
{

    QFile f ("sqlitedb/integral_bright.TXT");
    if (f.open(QIODevice::ReadOnly))
    {
        QTextStream in(&f);
        QString line;
        while (in.readLineInto(&line))
        {
            auto list = line.split(" ", QString::SkipEmptyParts);
            QSqlQuery query;
            query.exec(QString("UPDATE stars SET integral_bright = %1 WHERE hip_num = %2")
                       .arg(list[2].toDouble())
                    .arg(list[1].toInt()));
        }
    }
    else
    {
        qDebug() << "Не удалось считать файл интегральных яркостей";
    }
}


