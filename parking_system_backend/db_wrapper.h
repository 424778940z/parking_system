#ifndef DB_WRAPPER_H
#define DB_WRAPPER_H

#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlResult>
#include <QSqlTableModel>
#include <QVariantList>

#include <QDateTime>
#include <QTime>

#include <QDebug>

class db_wrapper
{
public:
    QSqlDatabase db;
    QSqlQuery query;
    QString path;
    bool isOpened = false;
    QSqlTableModel  *dbmd_current;
    QSqlTableModel  *dbmd_history;

    db_wrapper(QString db_path);
    ~db_wrapper();

    bool open();
    bool close();
    bool check(QString table_name);
    bool init();
    bool clear();

    bool update(bool in_out, QString plate);
    bool purge();
private:
    QString time_encoder(QDateTime time);
    QDateTime time_decoder(QString time_str);
};

#endif // DB_WRAPPER_H
