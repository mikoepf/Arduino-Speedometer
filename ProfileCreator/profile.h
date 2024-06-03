#ifndef PROFILE_H
#define PROFILE_H

#include <QDialog>
#include<QMessageBox>
#include<QSqlQuery>

namespace Ui {
class Profile;
}

class Profile : public QDialog
{
    Q_OBJECT

public:
    explicit Profile(QWidget *parent = nullptr, int pid = 0);
    ~Profile();

public slots:
    void save();
    void quit();
    void deleteProfile();

private:
    Ui::Profile *ui;
    int pk;
};


#endif // PROFILE_H

