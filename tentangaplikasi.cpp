#include "tentangaplikasi.h"
#include "ui_tentangaplikasi.h"

TentangAplikasi::TentangAplikasi(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TentangAplikasi)
{
    ui->setupUi(this);
}

TentangAplikasi::~TentangAplikasi()
{
    delete ui;
}

void TentangAplikasi::on_pushButton_tutup_clicked()
{
    accept ();
}
