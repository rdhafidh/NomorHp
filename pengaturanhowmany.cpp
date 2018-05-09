#include "pengaturanhowmany.h"
#include "ui_pengaturanhowmany.h"
#include <settinghandler.h>

PengaturanHowMany::PengaturanHowMany(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PengaturanHowMany)
{
    ui->setupUi(this);
    QVariant panjang=SettingHandler::getSetting ("table_paging","ditampilkan");
    bool isint=false;
    int num=panjang.toInt (&isint);
    if(isint){
        ui->spinBox->setValue (num);
    }
}

PengaturanHowMany::~PengaturanHowMany()
{
    delete ui;
}

void PengaturanHowMany::on_pushButton_simpan_clicked()
{
    SettingHandler::setSetting("pengaturan.ini","table_paging","ditampilkan",ui->spinBox->value ());
    accept ();
}

void PengaturanHowMany::on_pushButton_batal_clicked()
{
    reject ();
}
