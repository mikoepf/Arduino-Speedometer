#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Profile Management");
    sql = new QSqlQueryModel();

    // Display of the current directory
    QSqlQuery querystorage("select * from Storage");    // Reading the path/directory of the file "Profiles.cpp" form the database
    querystorage.first();
    path = querystorage.value(1).toString();    // Assigning the path/directory to the file "Profiles.cpp" form the database
    ui->dir_selectLineEdit->setText(path);  // Printing the path in the dir_selectLineEdit

    QObject::connect(ui->load_profilePushButton,SIGNAL(clicked(bool)),SLOT(LoadProfile()));
    QObject::connect(ui->load_defaultPushButton,SIGNAL(clicked(bool)),SLOT(LoadDefault()));
    QObject::connect(ui->delete_profilePushButton,SIGNAL(clicked(bool)),SLOT(Delete()));
    QObject::connect(ui->save_profilePushButton,SIGNAL(clicked(bool)),SLOT(Save()));
    QObject::connect(ui->dir_selectPushButton,SIGNAL(clicked(bool)),SLOT(DirSelect()));


    // Fill profile combobox with the contents of the Profiles table
    QSqlQuery queryprof("select * from Profiles");  // Gets the values from the Profile table
    while (queryprof.next())    // Iterates through all the objects/items in the list
    {

        // In the combobox is visible part (PName)"the names in the drop down menue"
        // and an invisible part (PID) "not shown in the drop down menue"
        QString visibleprof = queryprof.value(1).toString();
        ui->load_profileComboBox->addItem(visibleprof,queryprof.value(0));  // Stores the items in the drop down menu list
    }
    // Fill filter combobox with the contents of the Profiles table
    QSqlQuery queryfilter("select * from Filters"); // Gets the values from the Filter table
    while (queryfilter.next())   // Iterates through all the objects/items in the list
    {
        // In the combobox is visible part (FName)"the names in the drop down menue"
        // and an invisible part (FID) "not shown in the drop down menue"
        QString sichtbarfilter = queryfilter.value(1).toString();
        ui->filterComboBox->addItem(sichtbarfilter,queryfilter.value(0));   // Stores the items in the drop down menu list
    }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::LoadProfile()  // Method used to load profiles stored in the Profiles.db.
{

    QVariant variantprof = ui->load_profileComboBox->currentText(); // Reads the item currently selected/shown in the drop down menu
    QSqlQuery queryprof("select * from Profiles where PName = '" + variantprof.toString() + "'");   // Gets the values from the Profile table with the selected PName
    bool ret;   // Bool Variable to store the SQL request status
    if (queryprof.next())   // Iterates through all the objects/items in the list
    {   // Loads the retrieved values into the corresponding LineEdits and filter drop down menu
        ui->profile_nameLineEdit->setText(queryprof.value(1).toString());
        ui->min_rpmLineEdit->setText(queryprof.value(2).toString());
        ui->max_rpmLineEdit->setText(queryprof.value(3).toString());
        ui->sample_numberLineEdit->setText(queryprof.value(4).toString());
        int entry = ui->filterComboBox->findData(queryprof.value(5).toString());
        ui->filterComboBox->setCurrentIndex(entry); // Setts the index of the filter drop down menu which corresponds to the retrieved value(FID) from the Profiles query
        ret=queryprof.exec();   // Checks whether the query was succesfull
        if(!ret)
        {
            QMessageBox msg;    // Plotts a message, that the Selected-Profile could not be loaded!
            msg.setText("Selected-Profile could not be loaded!");
            msg.setWindowTitle("Error");
            msg.addButton("Ok",QMessageBox::YesRole);
            msg.exec();
        }
    }
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::LoadDefault()  // Method used to load the default profile stored in the Profiles.db.
{
    QSqlQuery queryprof("select * from Profiles where PName = 'Default'"); // Gets the values from the Profile table with the PName "Default"
    bool ret;   // Bool Variable to store the SQL request status

    if (queryprof.next())   // Iterates through all the objects/items in the list
    {   // Loads the retrieved values into the corresponding LineEdits and filter drop down menu
        ui->profile_nameLineEdit->setText(queryprof.value(1).toString());
        ui->min_rpmLineEdit->setText(queryprof.value(2).toString());
        ui->max_rpmLineEdit->setText(queryprof.value(3).toString());
        ui->sample_numberLineEdit->setText(queryprof.value(4).toString());
        int entry = ui->filterComboBox->findData(queryprof.value(5).toString());
        ui->filterComboBox->setCurrentIndex(entry);

    ret=queryprof.exec();   // Checks whether the query was succesfull
    if(!ret)
    {
        QMessageBox msg;    // Plotts a message, that the Default-Profile could not be loaded!
        msg.setText("Default-Profile could not be loaded!");
        msg.setWindowTitle("Error");
        msg.addButton("Ok",QMessageBox::YesRole);
        msg.exec();
    }
    }

}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::RefreshProfileCB() // Refreshes the list with the existing profiles in the combobox after each deletion, or creation of a profile in the Profiles.db.
{
    ui->load_profileComboBox->clear();  // Clears the list in the profil drop down menu
    QSqlQuery queryprofile("select * from Profiles"); // Gets the values from the Profile table
    while (queryprofile.next()) // Iterates through all the objects/items in the list
    {
        // In the combobox is visible part (PName)"the names in the drop down menue"
        // and an invisible part (PID) "not shown in the drop down menue"
        QString visibleprof = queryprofile.value(1).toString();
        ui->load_profileComboBox->addItem(visibleprof,queryprofile.value(0));   // Stores the items in the drop down menu list
    }

}
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::Delete()   // Method used to delete a selected profile from the Profiles.db.
{

    QVariant variantprofile = ui->load_profileComboBox->currentText();  // Reads the item currently selected/shown in the drop down menu

    if (variantprofile.toString() == "Default") //Checks whether the defaoult profile is selected in the drop down menu
    {   //  The default profile can only be adapted/changed, but not deleted
        QMessageBox msg;     // Plotts a message, that the Default-Profile can't be deleted!
        msg.setText("Default profile can´t be deleted!");
        msg.setWindowTitle("NOTE");
        msg.addButton("Ok",QMessageBox::YesRole);
        msg.exec();
    }
    else
    {
    QSqlQuery erase;
    bool ret;   // Bool Variable to store the SQL request status
    erase.exec("delete from Profiles where PName = '" + variantprofile.toString() + "'");   // Deletes the entries in the Profiles table with the selected PName

    ret=erase.exec();   // Checks whether the query was succesfull
    RefreshProfileCB(); // Refreshes the list with the existing profiles in the combobox after each deletion, or creation of a profile in the Profiles.db.
    if(!ret)
    {

        QMessageBox msg;    // Plotts a message, that the selected-Profile can't be deleted!
        msg.setText("Could not delete the Profile: " + variantprofile.toString());
        msg.setWindowTitle("Fehler");
        msg.addButton("Ok",QMessageBox::YesRole);
        msg.exec();
    }
    QMessageBox msg;    // Plotts a message, that the selected-Profile was successfully removed from the database
    msg.setText("Successfully deleted the Profile: " + variantprofile.toString());
    msg.setWindowTitle("Info");
    msg.addButton("Ok",QMessageBox::YesRole);
    msg.exec();
    }
    WriteFile();    // Method to write the profiles into the "Profiles.cpp".

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::Save() // Metod used to store changes in an existing profile, or to create a new profile, if there is no profile with the same profile name, in the Profiles.db.
{
    if(ui->min_rpmLineEdit->text().toInt() > ui->max_rpmLineEdit->text().toInt()-5 || ui->max_rpmLineEdit->text().toInt()>200 || ui->min_rpmLineEdit->text().toInt()<0) // Checks whether the Range of the min & maxrpm are 0-200 and  MINrpm <= MAXrpm - 5
    {
        QMessageBox msg;    // Plotts a message, that the requirements on the min and maxrpm are not met
        msg.setText("Range: 0-200, MINrpm <= MAXrpm - 5 !!!!!!!!");
        msg.setWindowTitle("Error");
        msg.addButton("Ok",QMessageBox::YesRole);
        msg.exec();
    }else if(ui->sample_numberLineEdit->text().toInt()>20 || ui->sample_numberLineEdit->text().toInt()<0)   //Checks whether the the sample number is within its range
    {
        QMessageBox msg;    // Plotts a message, that the sample number does not satisfy the range requirements
        msg.setText("Sample Number range: 0-20!!!!!!!!");
        msg.setWindowTitle("Error");
        msg.addButton("Ok",QMessageBox::YesRole);
        msg.exec();
    }else {
    QVariant variantfilter = ui->filterComboBox->currentText(); // Reads the item currently selected/shown in the drop down menu
    QSqlQuery queryfilter("select * from Filters where FName = '" + variantfilter.toString() + "'");    // Retrieves the values from the Profile table with the selected PName
    QSqlQuery queryprof("select * from Profiles where PName = '" + ui->profile_nameLineEdit->text() + "'"); // Retrieves the values from the Filter table with the selected FName

    bool ret;   // Bool Variable to store the SQL request status

    if(queryprof.next())    // Checks whether the selected profile already exists. If the profile exists, it will be updated, els a new profile will be created and stored in the database
    {
        QSqlQuery update;   // New query variable for Profiles update

        update.prepare("update Profiles set MINrpm=:minrpm, MAXrpm=:maxrpm, SNumber=:snumber, FilterFK=:filterfk where PID=:pid");  // sql command for Profile updates
        // Reads the values stored in the corresponding LineEdits and binds them to the placeholders ":placeholder"
        update.bindValue(":minrpm", ui->min_rpmLineEdit->text());
        update.bindValue(":maxrpm", ui->max_rpmLineEdit->text());
        update.bindValue(":snumber", ui->sample_numberLineEdit->text());
        if(queryfilter.next())  // Iterates through all the objects/items in the list
        {
        update.bindValue(":filterfk", queryfilter.value(0).toString());
        }
        update.bindValue(":pid",queryprof.value(0).toString());
        ret=update.exec();  // Checks whether the query was succesfull
        if(!ret)
        {
            QMessageBox msg;    // Plotts a message, that the selected profile could not be updated
            msg.setText("Error while updating the Profile: " + queryprof.value(1).toString());
            msg.setWindowTitle("Error");
            msg.addButton("Ok",QMessageBox::YesRole);
            msg.exec();
        }else{
        QMessageBox msg;    // Plotts a message, that the selected profile Successfully updated
        msg.setText("Successfully updated the Profile: " + queryprof.value(1).toString());
        msg.setWindowTitle("Info");
        msg.addButton("Ok",QMessageBox::YesRole);
        msg.exec();
        }
    }
    else
    {
        QSqlQuery insert;   // New query variable for creation of a new profile

        insert.prepare("insert into Profiles (PName,MINrpm,MAXrpm,SNumber,FilterFK) values (:pname,:minrpm,:maxrpm,:snumber,:filterfk)");   // sql command for new profile entry
        // Reads the values stored in the corresponding LineEdits and binds them to the placeholders ":placeholder"
        insert.bindValue(":pname", ui->profile_nameLineEdit->text());
         insert.bindValue(":minrpm", ui->min_rpmLineEdit->text());
         insert.bindValue(":maxrpm", ui->max_rpmLineEdit->text());
         insert.bindValue(":snumber", ui->sample_numberLineEdit->text());
         if(queryfilter.next())
         {
             insert.bindValue(":filterfk", queryfilter.value(0).toString());
         }

        ret=insert.exec();   // Checks whether the query was succesfull
         RefreshProfileCB();    // Refreshes the list with the existing profiles in the combobox after each deletion, or creation of a profile in the Profiles.db.
        if(!ret)
        {
             QMessageBox msg;   // Plotts a message, that the selected profile could not be created
             msg.setText("Error while creating the Profile: " + ui->profile_nameLineEdit->text());
             msg.setWindowTitle("Error");
             msg.addButton("Ok",QMessageBox::YesRole);
             msg.exec();
        }
        QMessageBox msg;    // Plotts a message, that the selected profile Successfully created
        msg.setText("Successfully created the Profile: " + ui->profile_nameLineEdit->text());
        msg.setWindowTitle("Info");
        msg.addButton("Ok",QMessageBox::YesRole);
        msg.exec();
    }
    WriteFile();    // Method to write the profiles into the "Profiles.cpp".

 }  //  END Range Validation

}
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::WriteFile()    // Method to prite the profiles into the "Profiles.cpp".
{
    // Create a file
    QString file_name = path + "Profiles.cpp";  // Stores the file name "Profiles.cpp" and the location "path" in a string
    QFile::remove(file_name);   // Deletes a file "Profiles.cpp" at the location "path"
    QFile file(file_name);  // Creates a new file "Profiles.cpp" at the location "path"
    if (file.open(QIODevice::ReadWrite)) {  // Opens the newly created file and initiates the writing
        QTextStream stream(&file);  // Interface for reading and writing files
        stream << "#include \"Profiles.h\" " << Qt::endl;
        stream << Qt::endl;

        QSqlQuery queryprofile("select * from Profiles");   // Gets the values from the Profile table
        size_t size = 0;    // Variable to store the amount of profiles stored in the list

        while (queryprofile.next()) // Iterates through all the objects/items in the list
        {
           ++size;  // Counts the number of the items in the list
        }
        stream << "const unsigned short int Profiles::pn = " + QString::number(size) + ";"<< Qt::endl;  // Writes text in file: int pn "number of entries/size of the array"
        queryprofile.first();   // Iterates back to the first entry in the list
        queryprofile.previous();    // Iterates back to the begin of the list
///////////////////////////////////////////////
        size_t i = 0;   // Variable to count the number of created entries in the array
        stream << "const String Profiles::profile_names [" + QString::number(size) + "] = {";   // Writes text in file: array with profile names
        while (queryprofile.next()) // Iterates through all the objects/items in the list
        {
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
        queryprofile.first();   // Iterates back to the first entry in the list
        queryprofile.previous();    // Iterates back to the begin of the list
///////////////////////////////////////////////
        stream << "const unsigned short int Profiles::min_rpms[" + QString::number(size) + "] = {"; // Writes text in file: array with min_rpms
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
        queryprofile.first();   // Iterates back to the first entry in the list
        queryprofile.previous();
//////////////////////////////////////////////
        stream << "const unsigned short int Profiles::max_rpms[" + QString::number(size) + "] = {"; // Writes text in file: array with max_rpms
        while (queryprofile.next()) // Iterates through all the objects/items in the list
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
        queryprofile.first();   // Iterates back to the first entry in the list
        queryprofile.previous();    // Iterates back to the begin of the list
//////////////////////////////////////////////
        stream << "const unsigned short int Profiles::sample_sizes[" + QString::number(size) + "] = {"; // Writes text in file: array with sample_sizes
        while (queryprofile.next()) // Iterates through all the objects/items in the list
        {
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
        queryprofile.first();   // Iterates back to the first entry in the list
        queryprofile.previous();    // Iterates back to the begin of the list
/////////////////////////////////////////////
        stream << "const String Profiles::filter_names[" + QString::number(size) + "] = {"; // Writes text in file: array with sample_sizes
        while (queryprofile.next()) // Iterates through all the objects/items in the list
        {
            QSqlQuery queryfilter("select * from Filters where FID = '" + queryprofile.value(5).toString() + "'");  // Gets the values from the Filters table with the FID corespindend to the currently iterated profile
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

               stream << "unsigned short int Profiles::count=0;" << Qt::endl;   //Writes text in file: count variable used to store the selected profile (in Arduino code)

////////////////////////////////////////////
        QMessageBox msg;    // Plotts a message, that the *Profiles.cpp* has been created and written
        msg.setText("*Profiles.cpp* has been created and written!");
        msg.setWindowTitle("Info");
        msg.addButton("Ok",QMessageBox::YesRole);
        msg.exec();
    }else{
        QMessageBox msg; // Plotts a message, that the *Profiles.cpp* could not be written
        msg.setText("The *Profiles.cpp* could not be written !");
        msg.setWindowTitle("Error");
        msg.addButton("OK", QMessageBox::YesRole);
        msg.exec();
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::DirSelect()    // Method to define a directory for the file "Profiles.cpp" to be written.
{
    bool ret;   // Bool Variable to store the SQL request status
    QFileDialog dialog(this);
    // Configure options
    dialog.setFileMode(QFileDialog::Directory);
    // Open the dialog window
    path = dialog.getExistingDirectory(this, "Choose directory for Profile.cpp storage", QDir::currentPath());  // Stores the directory/pth, slelected in the dialog window, into the "paht" variable as text/String
    path += "/";    // Inserts the "/" at the end of the text
    QSqlQuery updatestorage;
    updatestorage.prepare("update Storage set Data=:data where SID=:sid");  // sql command for Data update
        // Binds the "path" to the placeholders ":placeholder"
    updatestorage.bindValue(":data",path);
    updatestorage.bindValue(":sid", QString::number(1));    // in this case only the first entry is used

    ui->dir_selectLineEdit->setText(path);  // Printing the path in the dir_selectLineEdit
    ret=updatestorage.exec();   // Checks whether the query was succesfull
    if(!ret)
    {
        QMessageBox msg;    // Plotts a message, that there was an Error while updating the Storage / Data to new path:
        msg.setText("Error while updating the Storage / Data to new path: " + path);
        msg.setWindowTitle("Error");
        msg.addButton("Ok",QMessageBox::YesRole);
        msg.exec();
    }else{
    QMessageBox msg;    // Plotts a message, that the Storage / Data was Successfully updated to new path
    msg.setText("Successfully updated Storage / Data to new path: " + path);
    msg.setWindowTitle("Info");
    msg.addButton("Ok",QMessageBox::YesRole);
    msg.exec();
    }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////////////////////////////////////
MainWindow::~MainWindow()   // Main window destructor.
{
    delete ui;
    delete sql;
}


