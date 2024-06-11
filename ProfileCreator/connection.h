#ifndef CONNECTION_H
#define CONNECTION_H

#include <QtSql/QSqlDatabase>
#include<QtSQl/QSqlQuery>
#include <QMessageBox>
#include "QFile"
#include "QDebug"
#include <QTextStream>


static bool createconnection()  // Creates a database
{
    // Output of database types
    qDebug() << QSqlDatabase::drivers();    // Returns a list of drivers
    // Creating a database object
    QString dbName = "Profiles.db";   // Assignes a name for the database
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE"); //  Adds/instatiates a SQLdatabase
    db.setDatabaseName("Profiles.db");  // Assignes a name for the database
    if( QFile::exists(dbName)) // Checks wheher a dadabase with the assigned name already exists
    {
        QMessageBox msg;    // Plotts a message, that a database with the assigned name already exists
        msg.setText("Database *Profiles.db* has been loaded!");
        msg.setWindowTitle("Info");
        msg.addButton("Ok",QMessageBox::YesRole);
        msg.exec();
    if(!db.open())  // Checks whether a connection with the existing database could be established
        {
            QMessageBox msg;    // Plotts a message, that the connection with the existing database could not been established
            msg.setText("Unable to connect to *Profiles.db* !");
            msg.setWindowTitle("Error");
            msg.addButton("OK", QMessageBox::YesRole);
            msg.exec();
            return false;
        }
    }
    else
    {
    db.setDatabaseName("Profiles.db");  // Assignes a name for the database to be created
    if(!db.open())  // Checks whether a connection with the existing database could be established
    {
        QMessageBox msg;    // Plotts a message, that the connection with the newly created database could not been established
        msg.setText("Unable to open the database");
        msg.setWindowTitle("Error");
        msg.addButton("OK", QMessageBox::YesRole);
        msg.exec();
        return false;
    }
    QMessageBox msg;    // Plotts a message, that the newly created database is being set up
    msg.setText("Setting up new Database *Profiles.db*");
    msg.setWindowTitle("Info");
    msg.addButton("Ok",QMessageBox::YesRole);
    msg.exec();

    // Craetes tables in the newly created database
    QSqlQuery query;
    query.exec("drop table Profiles");
    query.exec("drop table Filters");

    // Craetes a Filter table in the newly created database
    qDebug() << query.exec("create table if not exists 'Filters' (\
                           'FID' integer not null unique, \
                           'FName' text not null unique,\
                           primary key ('FID' autoincrement)) \
                ");
                // Craetes a Profiles table in the newly created database
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
    // Craetes a Storage table in the newly created database. This table is used to store permanent parameters e.g: path to the created file
    qDebug() << query.exec("create table if not exists 'Storage' (\
                           'SID' integer not null unique, \
                           'Data' text not null, \
                           primary key ('SID' autoincrement)) \
                ");
    query.exec("delete from Profiles");
    query.exec("delete from Filters");
    query.exec("insert into Filters (FName) values ('AVR')");
    query.exec("insert into Filters (FName) values ('MED')");
    query.exec("insert into Filters (FName) values ('MIX')");
    query.exec("insert into Profiles (PName,MINrpm,MAXrpm,SNumber,FilterFK) values ('Default','30','80','6', 1)");
    query.exec("insert into Storage (Data) values ('../')");

    return true;
    }
    return true;
}




#endif // CONNECTION_H
