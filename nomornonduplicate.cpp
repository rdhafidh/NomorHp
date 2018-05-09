#include "nomornonduplicate.h"
#include "ui_nomornonduplicate.h"
#include<settinghandler.h>
#include <formtableview.h>

NomorNonDuplicate::NomorNonDuplicate(QWidget *parent) :
    QWidget(parent),mdb(nullptr),
    ui(new Ui::NomorNonDuplicate)
{
    ui->setupUi(this);
    placeholdercluster=tr("cluster daearah (ex jawa dll)");
    placeholdernomor=tr("input data nomor");
    ui->comboBox_cari_kategori_nomor_nested->hide ();
    ui->comboBox_cari_operator->hide ();
    ui->lineEdit_cari_nomor->show ();
    ui->lineEdit_cari_nomor->setPlaceholderText (placeholdernomor);
    ui->comboBox_cari_kategori_nomor->setCurrentIndex (0);
    connect(ui->formTableView,SIGNAL(deletedata(QString)),this,SLOT(entryRemove(QString)));
    connect(ui->formTableView,SIGNAL(refreshdata()),this,SLOT(entryRefresh()));
    ui->formTableView->setNamedKategoriExport ("nomor_non_duplicate");
}

NomorNonDuplicate::~NomorNonDuplicate()
{
    delete ui;
}

void NomorNonDuplicate::setconfigdb(dbbase::configdb *db)
{
    mdb=db;
    tableviewcommon com;
    sqlop opsql;
    QString tblbrg("nomor_nonduplicate");
    QVector<std::tuple<int, QString, QString> > fgrp;
    QString cari1_count="SELECT n.id,n.nomor ,(select pv.nama from  provider_nomor pv "
           " where "
            "pv.id=n.id_provider ) as nama_provider,(select k.nama from "
            "kategori_nomor k where k.id=n.id_kategori )as "
            "nama_kategori,n.tanggal_dibuat FROM "
            "nomor_nonduplicate n where n.id_kategori=1 and nomor like :0 ";
    QString cari1="SELECT n.id,n.nomor ,(select pv.nama from  provider_nomor pv "
            "where "
            "pv.id=n.id_provider ) as nama_provider,(select k.nama from "
            "kategori_nomor k where k.id=n.id_kategori )as "
            "nama_kategori,n.tanggal_dibuat FROM "
            "nomor_nonduplicate n where n.id_kategori=1 and nomor like :0 limit :1,:2 ";
    
    QString cari2_count="SELECT n.id,n.nomor ,(select pv.nama from  provider_nomor pv "
           " where "
           " pv.id=n.id_provider ) as nama_provider,(select k.nama from "
            "kategori_nomor k where k.id=n.id_kategori )as "
            "nama_kategori,n.tanggal_dibuat FROM " 
            "nomor_nonduplicate n where n.id_kategori=2 and nomor like :0 ";
    QString cari2="SELECT n.id,n.nomor ,(select pv.nama from  provider_nomor pv "
            "where "
           " pv.id=n.id_provider ) as nama_provider,(select k.nama from "
            "kategori_nomor k where k.id=n.id_kategori )as " 
            "nama_kategori,n.tanggal_dibuat FROM "
            "nomor_nonduplicate n where n.id_kategori=2 and nomor like :0 limit :1,:2 ";
    QString cari3="SELECT n.id,n.nomor ,(select pv.nama from  provider_nomor pv "
            "where " 
            "pv.id=n.id_provider ) as nama_provider,(select k.nama from " 
            "kategori_nomor k where k.id=n.id_kategori )as " 
            "nama_kategori,n.tanggal_dibuat FROM " 
            "nomor_nonduplicate n where n.id_kategori=3 and nomor like :0 limit :1,:2";
    QString cari3_count="SELECT n.id,n.nomor ,(select pv.nama from  provider_nomor pv "
            "where " 
            "pv.id=n.id_provider ) as nama_provider,(select k.nama from " 
            "kategori_nomor k where k.id=n.id_kategori )as " 
            "nama_kategori,n.tanggal_dibuat FROM " 
            "nomor_nonduplicate n where n.id_kategori=3 and nomor like :0 ";
    
    QString cari4="SELECT n.id,n.nomor ,(select pv.nama from  provider_nomor pv "
            "where "
            "pv.id=n.id_provider ) as nama_provider,(select k.nama from "
            "kategori_nomor k where k.id=n.id_kategori )as "
            "nama_kategori,n.tanggal_dibuat FROM "
            "nomor_nonduplicate n where n.id_kategori=:0 and n.id_provider=:1 limit :2,:3";
    
    QString cari4_count="SELECT n.id,n.nomor ,(select pv.nama from  provider_nomor pv "
            "where "
            "pv.id=n.id_provider ) as nama_provider,(select k.nama from "
            "kategori_nomor k where k.id=n.id_kategori )as "
            "nama_kategori,n.tanggal_dibuat FROM " 
            "nomor_nonduplicate n where n.id_kategori=:0 and n.id_provider=:1 ";
    
    fgrp.push_back(std::make_tuple(0, cari1_count, cari1));
    fgrp.push_back (std::make_tuple(1,cari2_count,cari2));
    fgrp.push_back (std::make_tuple(2,cari3_count,cari3));
    fgrp.push_back (std::make_tuple(3,cari4_count,cari4));
    opsql.setsql(
        "SELECT n.id,n.nomor ,(select pv.nama from  provider_nomor pv "
               " where " 
                "pv.id=n.id_provider ) as nama_provider,(select k.nama from "
                "kategori_nomor k where k.id=n.id_kategori )as "
                "nama_kategori,n.tanggal_dibuat FROM "
               " nomor_nonduplicate n limit :0, :1 ",
        tblbrg, fgrp);
    QStringList header;
    header << "ID"
           << "Nomor"
           << "Nama Provider"
           << "Nama Kategori"
           << "Tanggal Dibuat";
    priv::TableAccessLevel lvl;
    priv::acc adm = priv::acc::direktur;
    std::set<priv::act> actions = { priv::act::hapus,
                                   priv::act::select,
                                   priv::act::exportexcel};
    lvl.setsetting(adm, actions);
    int max=SettingHandler::getSetting ("table_paging","ditampilkan").toInt ();
    com.setsetting(*mdb, lvl, opsql, max, header, std::make_pair(false, 1));
    ui->formTableView->settableviewsetting(com);
    ui->formTableView->thisrefresh();
}

void NomorNonDuplicate::on_comboBox_cari_kategori_nomor_currentIndexChanged(int index)
{
    if(index==0){
        ui->comboBox_cari_kategori_nomor_nested->hide ();
        ui->comboBox_cari_operator->hide ();
        ui->lineEdit_cari_nomor->show ();
        ui->lineEdit_cari_nomor->setPlaceholderText (placeholdernomor);
    }
    if(index==1){
        ui->comboBox_cari_kategori_nomor_nested->hide ();
        ui->comboBox_cari_operator->hide ();
        ui->lineEdit_cari_nomor->show ();
        ui->lineEdit_cari_nomor->setPlaceholderText (placeholdernomor);
    }
    if(index==2){
        ui->comboBox_cari_kategori_nomor_nested->hide ();
        ui->comboBox_cari_operator->hide ();
        ui->lineEdit_cari_nomor->show ();
        ui->lineEdit_cari_nomor->setPlaceholderText (placeholdernomor);
    }
    if(index==3){
        ui->comboBox_cari_kategori_nomor_nested->show ();
        ui->lineEdit_cari_nomor->hide ();
        ui->comboBox_cari_operator->clear();
        utilproduk::sqlmodelwork wk1;
        QString filter = "select id,nama from provider_nomor";
        auto ret = wk1.selectnolmtReady(*mdb, filter);
        if (!ret.data().first) {
          error::dialog::emitdialogerror(
              this, tr("Galat"), tr("Terjadi galat ketika proses filter entri data "
                                    ". "
                                    "Klik selengkapnya untuk pesan lebih lanjut."),
              ret.message());
          return;
        }
        if (ret.data().second.size() == 0) {
          error::dialog::emitdialogwarning(
              this, tr("Galat"), tr("Data provider masih kosong, anda bisa upload "
                                    "data provider baru melalui db ms access  "
                                    ". "));
          return;
        }
        for (decltype(ret.data().second.size()) x = 0; x < ret.data().second.size();
             x++) {
          ui->comboBox_cari_operator->addItem(
              ret.data().second.at(x).value(1).toString(),
              ret.data().second.at(x).value(0));
        }
        ui->comboBox_cari_operator->show ();
    }
}

void NomorNonDuplicate::on_pushButton_cari_nomor_clicked()
{
    if(ui->comboBox_cari_kategori_nomor->currentIndex ()==0){
        ui->formTableView->setfilter (
                    std::make_pair(0,ui->lineEdit_cari_nomor->text ()));
    }
    if(ui->comboBox_cari_kategori_nomor->currentIndex ()==1){
        ui->formTableView->setfilter (
                    std::make_pair(1,ui->lineEdit_cari_nomor->text ()));
    }
    if(ui->comboBox_cari_kategori_nomor->currentIndex ()==2){
        ui->formTableView->setfilter (
                    std::make_pair(2,ui->lineEdit_cari_nomor->text ()));
    }
    
    if(ui->comboBox_cari_kategori_nomor->currentIndex ()==3){
        Param2VariantAndVariant param;
        param.index=3;
        param.filter1=ui->comboBox_cari_kategori_nomor_nested->currentIndex ()+1;
        param.filter2=ui->comboBox_cari_operator->currentData ();
        ui->formTableView->setfilterunik2 (param);
    }
}

void NomorNonDuplicate::entryRemove(const QString &id)
{
    QMessageBox ask(this);
    ask.setIcon(QMessageBox::Information);
  
    ask.setInformativeText(
        tr("Apakah anda yakin untuk menghapus Data Nomer dengan kode %1? ")
            .arg(id));
    ask.setText(tr("Konfirmasi penghapusan data nomor."));
    QPushButton *ya = ask.addButton(tr("Ya"), QMessageBox::ActionRole);
    QPushButton *tidak = ask.addButton(tr("Tidak jadi"), QMessageBox::ActionRole);
    ask.setDefaultButton(tidak);
    ask.exec();
    if (ask.clickedButton() == tidak) {
      return;
    }
    if (ask.clickedButton() == ya) {
      QVariantList row;
      row << id.toULongLong();
      utilproduk::sqlmodelwork del;
      QString hapus = "delete from nomor_nonduplicate where id= :0";
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
    
void NomorNonDuplicate::entryRefresh()
    {
        ui->formTableView->thisrefresh ();
    }
