#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Profile Management");
    sql = new QSqlQueryModel();

    QObject::connect(ui->load_profilePushButton,SIGNAL(clicked(bool)),SLOT(LoadProfile()));
    QObject::connect(ui->load_defaultPushButton,SIGNAL(clicked(bool)),SLOT(LoadDefault()));
    QObject::connect(ui->delete_profilePushButton,SIGNAL(clicked(bool)),SLOT(Delete()));
    QObject::connect(ui->save_profilePushButton,SIGNAL(clicked(bool)),SLOT(Save()));


    // Profil-Combobox mit dem Inhalt der Tabelle Profile füllen
    QSqlQuery queryprof("select * from Profiles");
    while (queryprof.next())
    {

        // in der Combobox steht sichbarer Teil (PName)
        // und ein unsichtbarer Teil (PID)
        QString visibleprof = queryprof.value(1).toString();
        ui->load_profileComboBox->addItem(visibleprof,queryprof.value(0));
    }

    QSqlQuery queryfilter("select * from Filters");
    while (queryfilter.next())
    {
        QString sichtbarfilter = queryfilter.value(1).toString();
        ui->filterComboBox->addItem(sichtbarfilter,queryfilter.value(0));
    }
  //  QObject::connect(ui->saveButton,SIGNAL(clicked(bool)),SLOT(save()));
  //  QObject::connect(ui->deleteButton,SIGNAL(clicked(bool)),SLOT(deleteKontakt()));
  //  QObject::connect(ui->quitButton,SIGNAL(clicked(bool)),SLOT(quit()));
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::LoadProfile()
{

    QVariant variantprof = ui->load_profileComboBox->currentText();
    QSqlQuery queryprof("select * from Profiles where PName = '" + variantprof.toString() + "'");

    if (queryprof.next())
    {
        ui->profile_nameLineEdit->setText(queryprof.value(1).toString());
        ui->min_rpmLineEdit->setText(queryprof.value(2).toString());
        ui->max_rpmLineEdit->setText(queryprof.value(3).toString());
        ui->sample_numberLineEdit->setText(queryprof.value(4).toString());
        int entry = ui->filterComboBox->findData(queryprof.value(5).toString());
        ui->filterComboBox->setCurrentIndex(entry);
    }
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::LoadDefault()
{

}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::RefreshProfileCB()
{
    //QSqlQuery queryprof("select * from Profiles where PName = '" + ui->profile_nameLineEdit->text() + "'");
    ui->load_profileComboBox->clear();
    QSqlQuery queryprofile("select * from Profiles");
    while (queryprofile.next())
    {
        QString visibleprof = queryprofile.value(1).toString();
        ui->load_profileComboBox->addItem(visibleprof,queryprofile.value(0));
    }

}
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::Delete()
{

    QVariant variantprofile = ui->load_profileComboBox->currentText();
 //   QSqlQuery queryprofile("select * from Filters where PName = '" + variantprofile.toString() + "'");

    QSqlQuery erase;
    bool ret;
    erase.exec("delete from Profiles where PName = '" + variantprofile.toString() + "'");

    ret=erase.exec();
    RefreshProfileCB();
    if(!ret)
    {

        QMessageBox msg;
        msg.setText("Deletion was unsuccessful PID: " + variantprofile.toString());
        msg.setWindowTitle("Fehler");
        msg.addButton("Ok",QMessageBox::YesRole);
        msg.exec();
    }

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::Save()
{

    QVariant variantfilter = ui->filterComboBox->currentText();
    QSqlQuery queryfilter("select * from Filters where FName = '" + variantfilter.toString() + "'");
    QSqlQuery queryprof("select * from Profiles where PName = '" + ui->profile_nameLineEdit->text() + "'");

    bool ret;

    if(queryprof.next())
    {
        QSqlQuery update;

        update.prepare("update Profiles set MINrpm=:minrpm, MAXrpm=:maxrpm, SNumber=:snumber, FilterFK=:filterfk where PID=:pid");

        update.bindValue(":minrpm", ui->min_rpmLineEdit->text());
        update.bindValue(":maxrpm", ui->max_rpmLineEdit->text());
        update.bindValue(":snumber", ui->sample_numberLineEdit->text());
        if(queryfilter.next())
        {
        update.bindValue(":filterfk", queryfilter.value(0).toString());
        }
        update.bindValue(":pid",queryprof.value(0).toString());
        ret=update.exec();
        RefreshProfileCB();
        if(!ret)
        {
            QMessageBox msg;
            msg.setText("Fehler beim updaten in der Datenbank filterfk: " + queryfilter.value(0).toString());
            msg.setWindowTitle("Fehler");
            msg.addButton("Ok",QMessageBox::YesRole);
            msg.exec();
        }
    }
    else
    {
        QSqlQuery insert;

        insert.prepare("insert into Profiles (PName,MINrpm,MAXrpm,SNumber,FilterFK) values (:pname,:minrpm,:maxrpm,:snumber,:filterfk)");
         insert.bindValue(":pname", ui->profile_nameLineEdit->text());
         insert.bindValue(":minrpm", ui->min_rpmLineEdit->text());
         insert.bindValue(":maxrpm", ui->max_rpmLineEdit->text());
         insert.bindValue(":snumber", ui->sample_numberLineEdit->text());
         if(queryfilter.next())
         {
             insert.bindValue(":filterfk", queryfilter.value(0).toString());
         }

        ret=insert.exec();
         RefreshProfileCB();
        if(!ret)
        {
            QMessageBox msg;
            msg.setText("Fehler beim speichern in der Datenbank -> filterfk" + queryfilter.value(0).toString());
            msg.setWindowTitle("Fehler");
            msg.addButton("Ok",QMessageBox::YesRole);
            msg.exec();
        }


    }



}
//////////////////////////////////////////////////////////////////////////////////////////////////////////
MainWindow::~MainWindow()
{
    delete ui;
    delete sql;
}


