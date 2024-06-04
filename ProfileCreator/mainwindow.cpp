#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Profile Management");
    sql = new QSqlQueryModel();

    // Profil-Combobox mit dem Inhalt der Tabelle Profile füllen
    QSqlQuery queryprof("select * from Profiles");
    while (queryprof.next())
    {
        // in der Combobox steht sichbarer Teil (PName)
        // und ein unsichtbarer Teil (PID)
        QString visibleprof = queryprof.value(1).toString();
        ui->load_profileComboBox->addItem(visibleprof,queryprof.value(0));
    }

    // Filter-Combobox mit dem Inhalt der Tabelle Filter füllen
    QSqlQuery queryfilter("select * from Filters");
    while (queryfilter.next())
    {
        // in der Combobox steht sichbarer Teil (FName)
        // und ein unsichtbarer Teil (FID)
        QString sichtbarfilter = queryfilter.value(1).toString();
        ui->filterComboBox->addItem(sichtbarfilter,queryfilter.value(0));
    }
  //  QObject::connect(ui->saveButton,SIGNAL(clicked(bool)),SLOT(save()));
  //  QObject::connect(ui->deleteButton,SIGNAL(clicked(bool)),SLOT(deleteKontakt()));
  //  QObject::connect(ui->quitButton,SIGNAL(clicked(bool)),SLOT(quit()));

}

MainWindow::~MainWindow()
{
    delete ui;
    delete sql;
}


