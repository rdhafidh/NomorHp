#include "asklogin.h"
#include "ui_asklogin.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <utilproduk.h>

AskLogin::AskLogin(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AskLogin)
{
    ui->setupUi(this);
    ui->checkBox->setChecked (false);
    ui->groupBox_detail_setting->setVisible (false);
}

AskLogin::~AskLogin()
{
    delete ui;
}

void AskLogin::on_checkBox_clicked(bool checked)
{
    ui->groupBox_detail_setting->setVisible (checked);
  
}

void AskLogin::on_pushButton_batal_clicked()
{
    this->askexit ();
   this->reject ();
}

void AskLogin::on_pushButton_login_clicked()
{
    /*
     * struktur config
     * 1. dbbackend
     * 2. hostname
     * 3. dbname
     * 4. username
     * 5. password
     * 6. connectopt
     * 7. port
     * 8. conname
     *
     **/
    dbbase::configdb mdb;
    mdb["dbbackend"]="QMYSQL";
    mdb["hostname"]=ui->lineEdit_alamat->text ();
    mdb["dbname"]=ui->lineEdit_namadb->text ();
    mdb["username"]=ui->lineEdit_username->text ();
    mdb["password"]=ui->lineEdit_password->text ();
    mdb["port"]=ui->spinBox_port->value ();
    utilproduk::sqlmodelwork wk;
    std::tuple<bool, QString, QString> ret=wk.testloginReady (mdb);
    if(std::get<0>(ret)==false){
        ui->label_error_msg->setText ("Login Database gagal");
        return;
    }else{
        ui->label_error_msg->setText ("Login Sukses");
    }
    QJsonObject obj;
    obj["username"]=ui->lineEdit_username->text ();
    obj["password"]=ui->lineEdit_password->text ();
    obj["dbname"]=ui->lineEdit_namadb->text ();
    obj["hostname"]=ui->lineEdit_alamat->text ();
    obj["port"]=ui->spinBox_port->value ();
    QJsonDocument doc(obj);
    this->asklogin (doc.toJson ());
    this->accept ();
}
