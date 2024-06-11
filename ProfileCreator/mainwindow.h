#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSql>
#include <QMessageBox>
#include <QSqlQueryModel>
#include <QFileDialog>
#include <QStandardItem>
#include<QTextStream>


QT_BEGIN_NAMESPACE
namespace Ui {  // Defines the namespace for the UI-related objects & methods.
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);  // Main window constructor.
    ~MainWindow();   // Main window destructor.

public slots:
    void LoadProfile(); // Method used to load profiles stored in the Profiles.db.
    void LoadDefault(); // Method used to load the default profile stored in the Profiles.db.
    void Delete();  // Method used to delete a selected profile from the Profiles.db.
    void Save();    // Metod used to store changes in an existing profile, or to create a new profile, if there is no profile with the same profile name, in the Profiles.db.
    void RefreshProfileCB();    // Refreshes the list with the existing profiles in the combobox after each deletion, or creation of a profile in the Profiles.db.
    void WriteFile();   // Method to write the profiles into the "Profiles.cpp".
    void DirSelect();   // Method to define a directory for the file "Profiles.cpp" to be written.


private:
    Ui::MainWindow *ui;
    QSqlQueryModel *sql;
    QString path;   // A Variable to store the path to the directory for the file "Profiles.cpp". This file should be stored in the arduino folder
};
#endif // MAINWINDOW_H




