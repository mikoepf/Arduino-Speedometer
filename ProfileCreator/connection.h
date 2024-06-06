#ifndef CONNECTION_H
#define CONNECTION_H

#include <QtSql/QSqlDatabase>
#include<QtSQl/QSqlQuery>
#include <QMessageBox>
#include "QFile"
#include "QDebug"
#include <QTextStream>


static bool createconnection()
{
    // Ausgabe der Typen von Datenbanken
    qDebug() << QSqlDatabase::drivers();    // Liefert eine Liste von Treibern
    // Anlegen eines Datenbankobjektes
    QString dbName = "Profiles.db";   //
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("Profiles.db");
    if( QFile::exists(dbName))
    {
        QMessageBox msg;
        msg.setText("Database *Profiles.db* has been loaded!");
        msg.setWindowTitle("Info");
        msg.addButton("Ok",QMessageBox::YesRole);
        msg.exec();
    if(!db.open())
        {
            QMessageBox msg;
            msg.setText("Unable to connect to *Profiles.db* !");
            msg.setWindowTitle("Error");
            msg.addButton("OK", QMessageBox::YesRole);
            msg.exec();
            return false;
        }
    }
    else
    {
    db.setDatabaseName("Profiles.db");
    if(!db.open())
    {
        QMessageBox msg;
        msg.setText("Unable to open the database");
        msg.setWindowTitle("Error");
        msg.addButton("OK", QMessageBox::YesRole);
        msg.exec();
        return false;
    }
    QMessageBox msg;
    msg.setText("Setting up new Database *Profiles.db*");
    msg.setWindowTitle("Info");
    msg.addButton("Ok",QMessageBox::YesRole);
    msg.exec();

    // Tabellen anlegen
    QSqlQuery query;
    query.exec("drop table Profiles");
    query.exec("drop table Filters");

    // Filter-Tabelle
    qDebug() << query.exec("create table if not exists 'Filters' (\
                           'FID' integer not null unique, \
                           'FName' text not null unique,\
                           primary key ('FID' autoincrement)) \
                ");
                // Profiles-Tabelle
                qDebug() << query.exec("create table if not exists 'Profiles' (\
                           'PID' integer not null unique, \
                           'PName' text not null, \
                           'MINrpm' text not null, \
                           'MAXrpm' text not null, \
                           'SNumber' integer not null, \
                           'FilterFK' integer not null, \
                           primary key ('PID' autoincrement) \
                           foreign key ('FilterFK') references 'Filters' ('FID'))\
                ");
    query.exec("delete from Profiles");
    query.exec("delete from Filters");
    query.exec("insert into Filters (FName) values ('AVR')");
    query.exec("insert into Filters (FName) values ('MED')");
    query.exec("insert into Filters (FName) values ('MIX')");
    query.exec("insert into Profiles (PName,MINrpm,MAXrpm,SNumber,FilterFK) values ('Default','30','80','6', 1)");


/*
                // Testdaten    => solange man entwickelt
                // wenn das Programm fertig ist, kann dieser Abschnitt gelöscht werden
    query.exec("delete from Profiles");
    query.exec("delete from Filters");
    query.exec("insert into Filters (FName) values ('AVR')");
    query.exec("insert into Filters (FName) values ('MED')");
    query.exec("delete from Filters where FName = 'MED'");
    query.exec("insert into Filters (FName) values ('MED')");
    query.exec("insert into Filters (FName) values ('MIX')");

    query.exec("insert into Profiles (PName,MINrpm,MAXrpm,SNumber,FilterFK) values ('Default','30','80','6', 1)");
    query.exec("insert into Profiles (PName,MINrpm,MAXrpm,SNumber,FilterFK) values ('Dirt_Fast','45','90','6', 3)");
    query.exec("insert into Profiles (PName,MINrpm,MAXrpm,SNumber,FilterFK) values ('Dirt_Slow','15','80','10', 3)");
    query.exec("delete from Profiles where PName = 'Dirt_Slow'");
    query.exec("insert into Profiles (PName,MINrpm,MAXrpm,SNumber,FilterFK) values ('Assetto F1','30','85','4', 1)");
    query.exec("insert into Profiles (PName,MINrpm,MAXrpm,SNumber,FilterFK) values ('Dirt_Slow','15','80','10', 4)");
*/
    return true;
    }
    return true;
}




#endif // CONNECTION_H
