#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Profile Management");
    sql = new QSqlQueryModel();

    QObject::connect(ui->loadButton,SIGNAL(clicked(bool)),SLOT(LoadProfile()));


    // Profil-Combobox mit dem Inhalt der Tabelle Profile füllen
    QSqlQuery queryprof("select * from Profiles");
    while (queryprof.next())
    {

        // in der Combobox steht sichbarer Teil (PName)
        // und ein unsichtbarer Teil (PID)
        QString visibleprof = queryprof.value(1).toString();
        //ui->load_profileComboBox->addItem(visibleprof);
        ui->load_profileComboBox->addItem(visibleprof,queryprof.value(0));
       // int currentindex = ui->load_profileComboBox->currentIndex();
       // ui->profile_nameLineEdit->setText(visibleprof[3]);

       // ui->profile_nameLineEdit->setText("visibleprof");
    }

    // Filter-Combobox mit dem Inhalt der Tabelle Filter füllen
    QSqlQuery queryfilter("select * from Filters");
    while (queryfilter.next())
    {
      //  int currentindex = ui->load_profileComboBox->currentIndex();
       // int test = 1;
        // in der Combobox steht sichbarer Teil (FName)
        // und ein unsichtbarer Teil (FID)
        QString sichtbarfilter = queryfilter.value(1).toString();
        ui->filterComboBox->addItem(sichtbarfilter,queryfilter.value(0));

        //ui->filterComboBox->addItem(sichtbarfilter,queryfilter.value(0));

       //  ui->filterComboBox->setCurrentIndex(currentindex);
    }
  //  QObject::connect(ui->saveButton,SIGNAL(clicked(bool)),SLOT(save()));
  //  QObject::connect(ui->deleteButton,SIGNAL(clicked(bool)),SLOT(deleteKontakt()));
  //  QObject::connect(ui->quitButton,SIGNAL(clicked(bool)),SLOT(quit()));
}

void MainWindow::LoadProfile()
{
   // int currentindex = ui->load_profileComboBox->currentIndex();

   // int fcurrentindex = ui->load_profileComboBox->currentData();

    QVariant variantprof = ui->load_profileComboBox->currentText();
    int plzid = variantprof.toInt();
    //int PID = query.value(1).toString();
   // std::string str = variant.toString();



    //QSqlQuery query("select * from Profiles where PID = 1");
    QSqlQuery queryprof("select * from Profiles where PName = '" + variantprof.toString() + "'");
    // QSqlQuery query("select * from Profiles where PName = 'Assetto F1'");
    if (queryprof.next())
    {

        ui->profile_nameLineEdit->setText(queryprof.value(1).toString());
        ui->min_rpmLineEdit->setText(queryprof.value(2).toString());
        ui->max_rpmLineEdit->setText(queryprof.value(3).toString());
        ui->sample_numberLineEdit->setText(queryprof.value(4).toString());

        //QSqlQuery queryprof("select * from Filters where FID = " [ queryfilter.value(5).toInt()] );
        //int entry = query.value(5).toInt();
        //ui->filterComboBox->setCurrentIndex(queryfilter.value(1).toInt());
        //ui->filterComboBox->setCurrentIndex(1);
       //QVariant variantfilter = ui->load_profileComboBox->currentData();
        QVariant variantfilter = ui->load_profileComboBox->currentData();
        //ui->sample_numberLineEdit->setText(variantfilter.toString());



        int entry = ui->filterComboBox->findData(queryprof.value(5).toString());
        ui->filterComboBox->setCurrentIndex(entry);

/*
//QSqlQuery queryfilter("select * from Filters where FID = " + variantfilter.toString() + "'" );
        QSqlQuery queryfilterr("select * from Filters where FID = " + queryprof.value(5).toString() + "'" );
        if (queryfilterr.next())
        {
        //ui->filterComboBox->setCurrentIndex(queryfilter.value(1).toInt());
        ui->filterComboBox->setCurrentText(queryfilterr.value(0).toString());
        ui->sample_numberLineEdit->setText(queryfilterr.value(0).toString());
        }
*/
    }
}

MainWindow::~MainWindow()
{
    delete ui;
    delete sql;
}


