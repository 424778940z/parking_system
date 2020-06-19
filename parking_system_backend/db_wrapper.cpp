#include "db_wrapper.h"

db_wrapper::db_wrapper(QString db_path)
{
    this->path = db_path;
}

db_wrapper::~db_wrapper()
{
    //this->close();
}

bool db_wrapper::open()
{
    if(QSqlDatabase::isDriverAvailable("QSQLITE"))
    {
        db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName(path);
        if(db.open())
        {
            query = QSqlQuery(db);
            this->isOpened = true;
            dbmd_current = new QSqlTableModel;
            dbmd_history = new QSqlTableModel;
            return true;
        }
        else
            return false;
    }
    else
        return false;
}

bool db_wrapper::close()
{
    delete dbmd_current;
    delete dbmd_history;
    db.close();
    isOpened = false;
    return true;
}

bool db_wrapper::check(QString table_name)
{
    if(db.tables(QSql::Tables).contains(table_name))
        return true;
    else
        return false;
}

bool db_wrapper::init()
{
    //database model init
    dbmd_current->setTable("vehicle_current");
    dbmd_current->setEditStrategy(QSqlTableModel::OnRowChange);
    dbmd_current->setHeaderData(0,Qt::Horizontal, "Plate Number");
    dbmd_current->setHeaderData(1,Qt::Horizontal, "Time In");
    dbmd_current->setHeaderData(2,Qt::Horizontal, "Time Out");
    dbmd_current->setHeaderData(4,Qt::Horizontal, "Notes");
    dbmd_history->setTable("vehicle_history");
    dbmd_history->setEditStrategy(QSqlTableModel::OnRowChange);
    dbmd_history->setHeaderData(1,Qt::Horizontal, "Plate Number");
    dbmd_history->setHeaderData(2,Qt::Horizontal, "Time In");
    dbmd_history->setHeaderData(3,Qt::Horizontal, "Time Out");
    dbmd_history->setHeaderData(4,Qt::Horizontal, "Time Total");
    dbmd_history->setHeaderData(5,Qt::Horizontal, "Notes");

    if(!this->check("vehicle_current"))
    {
        query.exec("CREATE TABLE vehicle_current (plate TEXT PRIMARY KEY NOT NULL UNIQUE, timestemp_in TEXT, timestemp_out TEXT, time_total TEXT, notes TEXT)");
    }
    //check header? nop, no time for that!
    if(!this->check("vehicle_history"))
    {
        query.exec("CREATE TABLE vehicle_history (id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL UNIQUE, plate TEXT, timestemp_in TEXT, timestemp_out TEXT, time_total TEXT, notes TEXT)");
    }
    //check header? nop, no time for that!
    if(this->check("vehicle_current") && this->check("vehicle_history"))
        return true;
    return false;
}

bool db_wrapper::clear()
{
    query.exec("DELETE FROM vehicle_current");
    query.exec("DELETE FROM vehicle_history");
    this->init();
    return true;// no error check for now
}

bool db_wrapper::update(bool in_out, QString plate)
{
    bool isExecSuccess;
    if(in_out) //true in
    {
        isExecSuccess = this->query.exec("INSERT INTO vehicle_current (plate, timestemp_in) VALUES( '" +
                                         plate + "', '" +
                                         this->time_encoder(QDateTime::currentDateTime()) + "' )"
                                         );
        if(!isExecSuccess)
        {
            qDebug() << query.lastError().text();
            return false;
        }

        // purge from current to history
        if(!this->purge())
            return false;

        return true;
    }
    else //false out
    {
        // update time out
        isExecSuccess = this->query.exec("UPDATE vehicle_current SET timestemp_out = '" +
                                         this->time_encoder(QDateTime::currentDateTime()) + "' " +
                                         "WHERE plate = '" +
                                         plate + "'"
                                         );
        if(!isExecSuccess)
        {
            qDebug() << query.lastError().text();
            return false;
        }

        // calculate time total
        isExecSuccess = this->query.exec("SELECT timestemp_in, timestemp_out FROM vehicle_current WHERE plate = '" +
                                         plate + "'"
                                         );
        if(!isExecSuccess)
        {
            qDebug() << query.lastError().text();
            return false;
        }
        if(!this->query.isSelect())
            return false;

        this->query.first();
        qint64 total_time_ms = this->time_decoder(this->query.value(0).toString()).msecsTo(this->time_decoder(this->query.value(1).toString()));

        // update time total
        isExecSuccess = this->query.exec("UPDATE vehicle_current SET time_total = '" +
                                         QTime::fromMSecsSinceStartOfDay(total_time_ms).toString("HH:mm:ss.zzz") + "' " +
                                         "WHERE plate is '" +
                                         plate + "'"
                                         );
        if(!isExecSuccess)
        {
            qDebug() << query.lastError().text();
            return false;
        }

        // purge from current to history
        if(!this->purge())
            return false;

        return true;
    }
}

bool db_wrapper::purge()
{
    bool isExecSuccess;

    isExecSuccess = this->query.exec("INSERT INTO vehicle_history (plate, timestemp_in, timestemp_out, time_total, notes) SELECT * FROM vehicle_current WHERE time_total IS NOT NULL");
    if(!isExecSuccess)
    {
        qDebug() << query.lastError().text();
        return false;
    }

    isExecSuccess = this->query.exec("DELETE FROM vehicle_current WHERE time_total IS NOT NULL");
    if(!isExecSuccess)
    {
        qDebug() << query.lastError().text();
        return false;
    }

    dbmd_current->select();
    dbmd_history->select();

    return true;
}



QString db_wrapper::time_encoder(QDateTime time)
{
    return time.toString("yyyyMMdd hh:mm:ss");
}

QDateTime db_wrapper::time_decoder(QString time_str)
{
    return QDateTime::fromString(time_str, "yyyyMMdd hh:mm:ss");
}

//query.exec("INSERT INTO vehicle (plate, timestemp_in, timestemp_out, notes) VALUES ('" + plate + "', '" + timestemp_in + "', '" + timestemp_out + "', '" + notes + "')"))
