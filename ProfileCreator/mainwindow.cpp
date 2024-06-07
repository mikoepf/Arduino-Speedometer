#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Profile Management");
    sql = new QSqlQueryModel();

    // Anzeige der aktuellen Directory
    // path = "C:/Users/KoepfM/source/QT/FileExplorer/";
    QSqlQuery querystorage("select * from Storage");
    querystorage.first();
    path = querystorage.value(1).toString();
   // readDir(path);   // "." ==> aktuelle Trajektorie
    ui->dir_selectLineEdit->setText(path);

    QObject::connect(ui->load_profilePushButton,SIGNAL(clicked(bool)),SLOT(LoadProfile()));
    QObject::connect(ui->load_defaultPushButton,SIGNAL(clicked(bool)),SLOT(LoadDefault()));
    QObject::connect(ui->delete_profilePushButton,SIGNAL(clicked(bool)),SLOT(Delete()));
    QObject::connect(ui->save_profilePushButton,SIGNAL(clicked(bool)),SLOT(Save()));
    QObject::connect(ui->dir_selectPushButton,SIGNAL(clicked(bool)),SLOT(dirSelect()));


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
    QSqlQuery queryprof("select * from Profiles where PName = 'Default'");
    bool ret;

    if (queryprof.next())
    {
        ui->profile_nameLineEdit->setText(queryprof.value(1).toString());
        ui->min_rpmLineEdit->setText(queryprof.value(2).toString());
        ui->max_rpmLineEdit->setText(queryprof.value(3).toString());
        ui->sample_numberLineEdit->setText(queryprof.value(4).toString());
        int entry = ui->filterComboBox->findData(queryprof.value(5).toString());
        ui->filterComboBox->setCurrentIndex(entry);

    ret=queryprof.exec();
    if(!ret)
    {
        QMessageBox msg;
        msg.setText("Default-Profile could not be loaded!");
        msg.setWindowTitle("Error");
        msg.addButton("Ok",QMessageBox::YesRole);
        msg.exec();
    }
    }

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

    if (variantprofile.toString() == "Default")
    {
        QMessageBox msg;
        msg.setText("Default profile can´t be deleted!");
        msg.setWindowTitle("NOTE");
        msg.addButton("Ok",QMessageBox::YesRole);
        msg.exec();
    }
    else
    {
    QSqlQuery erase;
    bool ret;
    erase.exec("delete from Profiles where PName = '" + variantprofile.toString() + "'");

    ret=erase.exec();
    RefreshProfileCB();
    if(!ret)
    {

        QMessageBox msg;
        msg.setText("Could not delete the Profile: " + variantprofile.toString());
        msg.setWindowTitle("Fehler");
        msg.addButton("Ok",QMessageBox::YesRole);
        msg.exec();
    }
    QMessageBox msg;
    msg.setText("Successfully deleted the Profile: " + variantprofile.toString());
    msg.setWindowTitle("Info");
    msg.addButton("Ok",QMessageBox::YesRole);
    msg.exec();
    }
    WriteFile();

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::Save()
{
      //  END Range Validation
    if(ui->min_rpmLineEdit->text().toInt() > ui->max_rpmLineEdit->text().toInt()-5 || ui->max_rpmLineEdit->text().toInt()>200 || ui->min_rpmLineEdit->text().toInt()<0)
    {
        QMessageBox msg;
        msg.setText("Range: 0-200, MINrpm <= MAXrpm - 5 !!!!!!!!");
        msg.setWindowTitle("Error");
        msg.addButton("Ok",QMessageBox::YesRole);
        msg.exec();
    }else if(ui->sample_numberLineEdit->text().toInt()>20 || ui->sample_numberLineEdit->text().toInt()<0)
    {
        QMessageBox msg;
        msg.setText("Sample Number range: 0-20!!!!!!!!");
        msg.setWindowTitle("Error");
        msg.addButton("Ok",QMessageBox::YesRole);
        msg.exec();
    }else {
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
        if(!ret)
        {
            QMessageBox msg;
            msg.setText("Error while updating the Profile: " + queryprof.value(1).toString());
            msg.setWindowTitle("Error");
            msg.addButton("Ok",QMessageBox::YesRole);
            msg.exec();
        }else{
        QMessageBox msg;
        msg.setText("Successfully updated the Profile: " + queryprof.value(1).toString());
        msg.setWindowTitle("Info");
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
             msg.setText("Error while creating the Profile: " + ui->profile_nameLineEdit->text());
             msg.setWindowTitle("Error");
             msg.addButton("Ok",QMessageBox::YesRole);
             msg.exec();
        }
        QMessageBox msg;
        msg.setText("Successfully created the Profile: " + ui->profile_nameLineEdit->text());
        msg.setWindowTitle("Info");
        msg.addButton("Ok",QMessageBox::YesRole);
        msg.exec();
    }
    WriteFile();

 }  //  END Range Validation

}
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::WriteFile()
{
    // Create a file
    QString file_name = path + "Profiles.cpp";   // QString filename="c:\\Data.txt" or QString filename="c:/Data.txt"
    QFile::remove(file_name);
    QFile file(file_name);
    if (file.open(QIODevice::ReadWrite)) {
        QTextStream stream(&file);
        stream << "#include \"Profiles.h\" " << Qt::endl;
        stream << Qt::endl;

        QSqlQuery queryprofile("select * from Profiles");
        size_t size = 0;// = sizeof(queryprofile);

        while (queryprofile.next())
        {
           ++size;
        }
        stream << "const unsigned short int Profiles::pn = " + QString::number(size) + ";"<< Qt::endl;
        queryprofile.first();
        queryprofile.previous();
///////////////////////////////////////////////
        size_t i = 0;
        stream << "const String Profiles::profile_names [" + QString::number(size) + "] = {";
        while (queryprofile.next())
        {
            //QString visibleprof = queryprofile.value(1).toString();
            stream << "\"" + queryprofile.value(1).toString() + "\"";
            if(i<size-1)
            {
            stream << ",";
            }else{
            stream << "};" << Qt::endl;
            }
            ++i;
        }
        i=0;
        queryprofile.first();
        queryprofile.previous();
///////////////////////////////////////////////
        stream << "const unsigned short int Profiles::min_rpms[" + QString::number(size) + "] = {";
        while (queryprofile.next())
        {
            stream << queryprofile.value(2).toString();
            if(i<size-1)
            {
                stream << ",";
            }else{
                stream << "};" << Qt::endl;
            }
            ++i;
        }
        i=0;
        queryprofile.first();
        queryprofile.previous();
//////////////////////////////////////////////
        stream << "const unsigned short int Profiles::max_rpms[" + QString::number(size) + "] = {";
        while (queryprofile.next())
        {
            stream <<  queryprofile.value(3).toString();
            if(i<size-1)
            {
                stream << ",";
            }else{
                stream << "};" << Qt::endl;
            }
            ++i;
        }
        i=0;
        queryprofile.first();
        queryprofile.previous();
//////////////////////////////////////////////
        stream << "const unsigned short int Profiles::sample_sizes[" + QString::number(size) + "] = {";
        while (queryprofile.next())
        {
            //QString visibleprof = queryprofile.value(1).toString();
            stream <<  queryprofile.value(4).toString();
            if(i<size-1)
            {
                stream << ",";
            }else{
                stream << "};" << Qt::endl;
            }
            ++i;
        }
        i=0;
        queryprofile.first();
        queryprofile.previous();
/////////////////////////////////////////////
        stream << "const String Profiles::filter_names[" + QString::number(size) + "] = {";
        while (queryprofile.next())
        {
            QSqlQuery queryfilter("select * from Filters where FID = '" + queryprofile.value(5).toString() + "'");
            queryfilter.first();
            stream << "\"" + queryfilter.value(1).toString() + "\"";
            if(i<size-1)
            {
                stream << ",";
            }else{
                stream << "};" << Qt::endl;
            }
            ++i;
        }

////////////////////////////////////////////

               stream << "unsigned short int Profiles::count=0;" << Qt::endl;

////////////////////////////////////////////
        QMessageBox msg;
        msg.setText("*Profiles.cpp* has been created and written!");
        msg.setWindowTitle("Info");
        msg.addButton("Ok",QMessageBox::YesRole);
        msg.exec();
    }else{
        QMessageBox msg;
        msg.setText("The *Profiles.cpp* could not be written !");
        msg.setWindowTitle("Error");
        msg.addButton("OK", QMessageBox::YesRole);
        msg.exec();
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::dirSelect()
{
    bool ret;
    QFileDialog dialog(this);
    // Optionen konfigurieren
    dialog.setFileMode(QFileDialog::Directory);
    // Öffnen des Dialogs
    path = dialog.getExistingDirectory(this, "Choose directory for Profile.cpp storage", QDir::currentPath());
    path += "/";
    QSqlQuery updatestorage;
    updatestorage.prepare("update Storage set Data=:data where SID=:sid");

    updatestorage.bindValue(":data",path);
    updatestorage.bindValue(":sid", QString::number(1));

    ui->dir_selectLineEdit->setText(path);
    ret=updatestorage.exec();
    if(!ret)
    {
        QMessageBox msg;
        msg.setText("Error while updating the Storage / Data to new path: " + path);
        msg.setWindowTitle("Error");
        msg.addButton("Ok",QMessageBox::YesRole);
        msg.exec();
    }else{
    QMessageBox msg;
    msg.setText("Successfully updated Storage / Data to new path: " + path);
    msg.setWindowTitle("Info");
    msg.addButton("Ok",QMessageBox::YesRole);
    msg.exec();
    }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////////////////////////////////////
MainWindow::~MainWindow()
{
    delete ui;
    delete sql;
}


