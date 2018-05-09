#include "halamanprovider.h"
#include "ui_halamanprovider.h"
#include <formtableview.h>
#include <settinghandler.h>

HalamanProvider::HalamanProvider(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HalamanProvider),mdb(nullptr)
{
    ui->setupUi(this);
    connect(ui->formTableView,SIGNAL(adddata()),this,SLOT(add_data_provider()));
    connect(ui->formTableView,SIGNAL(editdata(QString)),this,SLOT(edit_data_byid(QString)));
    connect(ui->formTableView,SIGNAL(deletedata(QString)),this,SLOT(hapus_edit_byid(QString)));
    ui->stackedWidget->setCurrentIndex (0);
    ui->pushButton_simpan->setEnabled (false);
}

HalamanProvider::~HalamanProvider()
{
    delete ui;
}

void HalamanProvider::setdbconfig(dbbase::configdb *db)
{
    mdb=db;
    
    if (mdb) {
      tableviewcommon com;
      sqlop opsql;
      QString tblbrg("provider_nomor");
      QVector<std::tuple<int, QString, QString> > fgrp;
      opsql.setsql(
          "select p.id,p.nama,p.tanggal_dibuat from provider_nomor p limit :0,:1; ",
          tblbrg, fgrp);
      QStringList header;
      header << "ID"
             << "Nama Provider"
             << "Tanggal Dibuat";
      priv::TableAccessLevel lvl;
      priv::acc adm = priv::acc::direktur;
      std::set<priv::act> actions = {priv::act::edit, priv::act::hapus,
                                     priv::act::select, priv::act::tambah};
      lvl.setsetting(adm, actions);
      int max=SettingHandler::getSetting ("table_paging","ditampilkan").toInt ();
      com.setsetting(*mdb, lvl, opsql, max, header, std::make_pair(false, 1));
      ui->formTableView->settableviewsetting(com);
      ui->formTableView->thisrefresh();
    }
}

void HalamanProvider::on_lineEdit_textChanged(const QString &arg1)
{
    if(arg1.isEmpty ()){
        ui->pushButton_simpan->setEnabled (false);
    }else{
        ui->pushButton_simpan->setEnabled (true);
    }
}

void HalamanProvider::on_pushButton_simpan_clicked()
{
    utilproduk::sqlmodelwork wk1;
    if(modeadd_or_edit){
        QString sql="insert into provider_nomor(`nama`,`tanggal_dibuat`) values(:0,:1);";
        QVariantList row;
        row<<ui->lineEdit->text ();
        row<<QDate::currentDate ();
        auto ret=wk1.editReady (*mdb,sql,row);
        if(!ret.first){
            error::dialog::emitdialogerror (this,tr("Galat")
                                            ,tr("Terjadi galat ketika tambah data provider. Klik selengkapnya untuk mengetahui penyebabnya.")
                                            ,ret.second);
            return;
        }
    }else{
        QString sql="update provider_nomor set nama =:0 where id=:1;";
        QVariantList row;
        row<<ui->lineEdit->text ();
        row<<last_edit_id;
        auto ret=wk1.editReady (*mdb,sql,row);
        if(!ret.first){
            error::dialog::emitdialogerror (this,tr("Galat")
                                            ,tr("Terjadi galat ketika edit data provider %1. Klik selengkapnya untuk mengetahui penyebabnya.").arg (last_edit_id)
                                            ,ret.second);
            return;
        }
    }
    ui->stackedWidget->setCurrentIndex (0);
    ui->formTableView->thisrefresh ();
}

void HalamanProvider::on_pushButton_batal_clicked()
{
    ui->lineEdit->clear ();
    ui->stackedWidget->setCurrentIndex (0);
}

void HalamanProvider::add_data_provider()
{
    modeadd_or_edit=true;
    ui->lineEdit->clear ();
    ui->stackedWidget->setCurrentIndex (1);
}

void HalamanProvider::edit_data_byid(const QString &id)
{
    last_edit_id=id.toULongLong ();
    modeadd_or_edit=false;
    QString sql="select p.nama from provider_nomor p where p.id=:0;";
    utilproduk::sqlmodelwork wk1;
    auto ret=wk1.filterequalbysinglegroupReady (*mdb,sql,last_edit_id,
                                                qMakePair(0,1),true);
    if(!ret.data ().first){
        error::dialog::emitdialogerror (this,tr("Galat")
                                        ,tr("Terjadi galat ketika edit data provider %1. Klik selengkapnya untuk mengetahui penyebabnya.").arg (id)
                                        ,ret.message ());
        return;
    }
    if(ret.data ().second.size ()==0){
        error::dialog::emitdialogwarning (this,tr("Galat")
                                        ,tr("Data yang akan diedit sudah tidak ada dalam database.").arg (id)
                                        );
        return;
    }
    ui->lineEdit->setText (ret.data ().second.at (0).value(0).toString());
    ui->stackedWidget->setCurrentIndex (1);
}

void HalamanProvider::hapus_edit_byid(const QString &id)
{
    QMessageBox ask(this);
    ask.setIcon(QMessageBox::Information);
  
    ask.setInformativeText(
        tr("Apakah anda yakin untuk menghapus Data dengan kode %1? ")
            .arg(id));
    ask.setText(tr("Konfirmasi penghapusan data provider ."));
    QPushButton *ya = ask.addButton(tr("Ya"), QMessageBox::ActionRole);
    QPushButton *tidak = ask.addButton(tr("Tidak jadi"), QMessageBox::ActionRole);
    ask.setDefaultButton(tidak);
    ask.exec();
    if (ask.clickedButton() == tidak) {
      return;
    }
    if (ask.clickedButton() == ya) {
      QVariantList row;
      row << id.toULongLong ();
      utilproduk::sqlmodelwork del;
      QString hapus = "delete from provider_nomor where id= :0";
      std::pair<bool, QString> ret = del.editReady(*mdb, hapus, row);
      if (!ret.first) {
        error::dialog::emitdialogerror(
            this, tr("Galat"), tr("Terjadi galat ketika proses hapus entri data "
                                  " dengan kode %1. "
                                  "Klik selengkapnya untuk pesan lebih lanjut.")
                                   .arg(id),
            ret.second);
        return;
      }
      ui->formTableView->thisrefresh();
    }
}
