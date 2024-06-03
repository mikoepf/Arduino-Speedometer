#include "profile.h"
#include "ui_profile.h"

Profile::Profile(QWidget *parent, int fid) :
    QDialog(parent),
    ui(new Ui::Profile)
{
    ui->setupUi(this);
    this->setWindowTitle("Profile editieren");

    // Combobox mit dem Inhalt der Tabelle Postleitzahlen füllen
    QSqlQuery queryplz("select * from Postleitzahlen");
    while (queryplz.next())
    {
        // in der Combobox steht sichtbarer Teil (PLZ + Ort)
        // und ein unsichtbarer Teil (PID)
        QString sichtbar = queryplz.value(1).toString() + " - " + queryplz.value(2).toString();
        ui->postleitzahlComboBox->addItem(sichtbar,queryplz.value(0));
    }

    QObject::connect(ui->saveButton,SIGNAL(clicked(bool)),SLOT(save()));
    QObject::connect(ui->deleteButton,SIGNAL(clicked(bool)),SLOT(deleteKontakt()));
    QObject::connect(ui->quitButton,SIGNAL(clicked(bool)),SLOT(quit()));

    // Abfrage new oder Edit
    if(kid)
    {
        // Edit Modus => einen Datensatz abfragen
        QSqlQuery query ("select * from Kontakte where KID = " + QString::number(kid));
        if(query.next())
        {
            // DB ist abgefragt, die Daten müssen an die Oberfläche geschrieben werden
            ui->nameLineEdit->setText(query.value(1).toString());
            ui->adresseLineEdit->setText(query.value(2).toString());
            //  Eintrag in der Combobox auswählen
            int entry = ui->postleitzahlComboBox->findData(query.value(3).toInt());
            ui->postleitzahlComboBox->setCurrentIndex(entry);
        }
    }
    // Speicehrn der ID
    pk = kid;
}

Kontakt::~Kontakt()
{
    delete ui;
}

void Kontakt::save()
{
    //  Abfrage der Combobox
    // currentindex => Index des Eintrages, der selectiert wurde
    int currentindex = ui->postleitzahlComboBox->currentIndex();
    //  Daten für diesen Index holen
    QVariant variant = ui->postleitzahlComboBox->itemData(currentindex);
    int plzid = variant.toInt();
    bool ret;
    if(pk)
    {
        //  Edit eines Kontaktes
        QSqlQuery update;
        update.prepare("update Kontakte set KName=:name, KAdr=:adr, KPlzFK=:plzid where KID=:kid");
        update.bindValue(":name", ui->nameLineEdit->text());
        update.bindValue(":adr", ui->adresseLineEdit->text());
        update.bindValue(":plzid", plzid);
        update.bindValue(":kid",pk);
        ret=update.exec();
    }
    else
    {

        // Speichern des Kontaktes
        QSqlQuery insert;
        insert.prepare("insert into Kontakte (KName, KAdr,KPlzFK) values (:name,:adresse,:plzid)");
        // Einfügen der Daten in die Platzhalter
        insert.bindValue(":name", ui->nameLineEdit->text());
        insert.bindValue(":adresse", ui->adresseLineEdit->text());
        insert.bindValue(":plzid", plzid);  // dies ist der Wert aus der Combobox
        ret=insert.exec();
        if(!ret)
        {
            QMessageBox msg;
            msg.setText("Fehler beim speichern in der Datenbank");
            msg.setWindowTitle("Fehler");
            msg.addButton("Ok",QMessageBox::YesRole);
            msg.exec();
        }
    }


}



void Kontakt::quit()
{
    this->close();
}

void Kontakt::deleteKontakt()
{

}
