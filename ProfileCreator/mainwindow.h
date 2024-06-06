#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSql>
#include <QMessageBox>
#include <QSqlQueryModel>
#include <QFileDialog>
#include <QStandardItem>
#include<QTextStream>

//#include "profile.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void LoadProfile();
    void LoadDefault();
    void Delete();
    void Save();
    void RefreshProfileCB();
    void WriteFile();
    void dirSelect();   ///////////


private:
    Ui::MainWindow *ui;
    QSqlQueryModel *sql;
 //   void readDir(QString path); ////////////////
    QString path;   /////////////////
};
#endif // MAINWINDOW_H




