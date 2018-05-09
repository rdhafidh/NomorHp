#include "halamanpage.h"
#include <formtableview.h>
#include <QRegExpValidator>
#include "ui_halamanpage.h"
#include <settinghandler.h>

HalamanPage::HalamanPage(QWidget *parent)
    : QWidget(parent), ui(new Ui::HalamanPage), mdb(nullptr) {
  ui->setupUi(this);
  ui->stackedWidget->setCurrentIndex(0);
  ui->lineEdit_inputcari->hide();
  connect(ui->formTableView, SIGNAL(deletedata(QString)), this,
          SLOT(removeEntry(QString)));
  connect(ui->formTableView, SIGNAL(adddata()), this, SLOT(addentrybaru()));
  connect(ui->formTableView, SIGNAL(editdata(QString)), this,
          SLOT(editEntry(QString)));
  connect(ui->formTableView,SIGNAL(beforeExportExcelHappen()),this,SLOT(setDataViewExportNameKategori()));
  QRegExp xp("[0-9]*");
  numvalid = new QRegExpValidator(this);
  numvalid->setRegExp(xp);
  ui->lineEdit_inputnomor->setValidator(numvalid);
  ui->lineEdit_inputnomor->setMaxLength(12);
   ui->lineEdit_inputcari->show ();
  ui->comboBox_cari_provider->hide();
  ui->comboBox_kategori_nomor->hide ();
  placeholdercluster=tr("input data cluster daerah(ex: jawa dll)");
  placeholdernomor=tr("input data nomor");
   ui->lineEdit_inputcari->setPlaceholderText (placeholdernomor);
  ui->formTableView->setNamedKategoriExport ("master_nomor");
}

HalamanPage::~HalamanPage() { delete ui; }

void HalamanPage::setdbconfig(dbbase::configdb *db) {
  mdb = db;
  if (mdb) {
    tableviewcommon com;
    sqlop opsql;
    QString tblbrg("nomor_telp");
    QVector<std::tuple<int, QString, QString> > fgrp;
    QString cari1 =
        "SELECT n.id,n.nama, n.nomor ,(select pv.nama from  provider_nomor pv "
        "where "
        "pv.id=n.id_provider ) as nama_provider,n.cluster,(select k.nama from "
        "kategori_nomor k where k.id=n.id_kategori )as "
        "nama_kategori,n.tanggal_dibuat FROM "
        "nomor_telp n where n.id_kategori=1 and n.nomor like :0 limit :1, :2";

    QString cari1_count =
        "SELECT n.id,n.nama, n.nomor ,(select pv.nama from  provider_nomor pv "
        "where "
        "pv.id=n.id_provider ) as nama_provider,n.cluster,(select k.nama from "
        "kategori_nomor k where k.id=n.id_kategori )as "
        "nama_kategori,n.tanggal_dibuat FROM "
        "nomor_telp n where n.id_kategori=1 and n.nomor like :0 ";
    QString cari2 =
        "SELECT n.id,n.nama ,n.nomor ,(select pv.nama from  provider_nomor pv "
        "where "
        "pv.id=n.id_provider ) as nama_provider,n.cluster,(select k.nama from "
        "kategori_nomor k where k.id=n.id_kategori )as "
        "nama_kategori,n.tanggal_dibuat FROM "
        "nomor_telp n where  n.nomor like :0  and n.id_kategori=2 limit :1, "
        ":2";

    QString cari2_count =
        "SELECT n.id ,n.nomor ,(select pv.nama from  provider_nomor pv where "
        "pv.id=n.id_provider ) as nama_provider,n.cluster,(select k.nama from "
        "kategori_nomor k where k.id=n.id_kategori )as "
        "nama_kategori,n.tanggal_dibuat FROM "
        "nomor_telp n where   n.nomor like :0   and n.id_kategori=2 ";

    QString cari3 =
        "SELECT n.id,n.nama, n.nomor ,(select pv.nama from  provider_nomor pv "
        "where "
        "pv.id=n.id_provider ) as nama_provider,n.cluster,(select k.nama from "
        "kategori_nomor k where k.id=n.id_kategori )as "
        "nama_kategori,n.tanggal_dibuat FROM "
        "nomor_telp n where    n.nomor like  :0  and n.id_kategori=3 limit "
        ":1, :2";

    QString cari3_count =
        "SELECT n.id, n.nomor ,(select pv.nama from  provider_nomor pv where "
        "pv.id=n.id_provider ) as nama_provider,n.cluster,(select k.nama from "
        "kategori_nomor k where k.id=n.id_kategori )as "
        "nama_kategori,n.tanggal_dibuat FROM "
        "nomor_telp n where     n.nomor like  :0 and n.id_kategori=3";

    // per cluster
    QString cari4 =
        "SELECT n.id,n.nama, n.nomor ,(select pv.nama from  provider_nomor pv "
        "where "
        "  pv.id=n.id_provider ) as nama_provider,n.cluster,(select k.nama "
        "from "
        "  kategori_nomor k where k.id=n.id_kategori )as "
        " nama_kategori,n.tanggal_dibuat FROM "
        " nomor_telp n where n.cluster like :0 and n.id_kategori=:1 limit :2,:3;";

    QString cari4_count =
        "SELECT n.id, n.nomor ,(select pv.nama from  provider_nomor pv where "
        "  pv.id=n.id_provider ) as nama_provider,n.cluster,(select k.nama "
        "from "
        "  kategori_nomor k where k.id=n.id_kategori )as "
        " nama_kategori,n.tanggal_dibuat FROM "
        " nomor_telp n where n.cluster like :0 and n.id_kategori=:1 ";

    // per provider
    QString cari5 =
        "SELECT n.id,n.nama, n.nomor ,(select pv.nama from  provider_nomor pv "
        "where "
        "  pv.id=n.id_provider ) as nama_provider,n.cluster,(select k.nama "
        "from "
        "kategori_nomor k where k.id=n.id_kategori )as "
        " nama_kategori,n.tanggal_dibuat FROM "
        " nomor_telp n where n.id_provider=:0 and n.id_kategori=:1 limit :2,:3; ";

    QString cari5_count =
        "SELECT n.id, n.nomor ,(select pv.nama from  provider_nomor pv where "
        "  pv.id=n.id_provider ) as nama_provider,n.cluster,(select k.nama "
        "from "
        "kategori_nomor k where k.id=n.id_kategori )as "
        " nama_kategori,n.tanggal_dibuat FROM "
        " nomor_telp n where n.id_provider=:0 and n.id_kategori=:1";

    fgrp.push_back(std::make_tuple(0, cari1_count, cari1));
    fgrp.push_back(std::make_tuple(1, cari2_count, cari2));
    fgrp.push_back(std::make_tuple(2, cari3_count, cari3));
    fgrp.push_back(std::make_tuple(3, cari4_count, cari4));
    fgrp.push_back(std::make_tuple(4, cari5_count, cari5));

    opsql.setsql(
        "SELECT n.id,n.nama, n.nomor ,(select pv.nama from  provider_nomor pv "
        "where "
        "pv.id=n.id_provider ) as nama_provider,n.cluster,(select k.nama from "
        "kategori_nomor k where k.id=n.id_kategori )as "
        "nama_kategori,n.tanggal_dibuat FROM "
        "nomor_telp n limit :0, :1 ",
        tblbrg, fgrp);
    QStringList header;
    header << "ID"
           << "Nama"
           << "Nomor"
           << "Nama Provider"
           << "Cluster"
           << "Nama Kategori"
           << "Tanggal Dibuat";
    priv::TableAccessLevel lvl;
    priv::acc adm = priv::acc::direktur;
    std::set<priv::act> actions = {priv::act::edit, priv::act::hapus,
                                   priv::act::select, priv::act::tambah,
                                   priv::act::exportexcel};
    lvl.setsetting(adm, actions);
    int max=SettingHandler::getSetting ("table_paging","ditampilkan").toInt ();
    com.setsetting(*mdb, lvl, opsql, max, header, std::make_pair(false, 1));
    ui->formTableView->settableviewsetting(com);
    ui->formTableView->thisrefresh();
  }
}

void HalamanPage::askrefresh()
{
    ui->formTableView->thisrefresh ();
}

void HalamanPage::on_lineEdit_inputcari_returnPressed() {
  on_pushButton_cari_clicked();
}

void HalamanPage::addentrybaru() {
  modeadd_or_edit = true;
  ui->lineEdit_kota->clear();
  ui->pushButton_simpan->setEnabled(false);
  ui->stackedWidget->setCurrentIndex(1);
}

void HalamanPage::removeEntry(const QString &id) {
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
    QString hapus = "delete from nomor_telp where id= :0";
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

void HalamanPage::editEntry(const QString &id) {
  ui->lineEdit_inputnomor->clear ();
  ui->lineEdit_kota->clear ();
  modeadd_or_edit = false;
  last_id_edit = id.toULongLong();
  QString sql = "select n.cluster,n.nomor from nomor_telp  n WHERE n.id=:0 limit :1,:2";
  utilproduk::sqlmodelwork wk1;
  auto ret = wk1.filterequalbysinglegroupReady(*mdb, sql, id.toULongLong(),
                                               qMakePair(0, 1), true);
  if (!ret.data().first) {
    error::dialog::emitdialogerror(
        this, tr("Galat"), tr("Terjadi galat ketika proses edit entri data "
                              " dengan kode %1. "
                              "Klik selengkapnya untuk pesan lebih lanjut.")
                               .arg(id),
        ret.message());
    return;
  }
  if (ret.data().second.size() == 0) {
    error::dialog::emitdialogerror(
        this, tr("Galat"),
        tr("Edit entri data "
           " dengan kode %1 sudah tidak bisa ditemukan lagi dalam database. "
           "Anda bisa memilih data yang lainya untuk diedit.")
            .arg(id),
        ret.message());
    return;
  }
  ui->lineEdit_inputnomor->setText(ret.data().second.at(0).value(1).toString());
  ui->lineEdit_kota->setText(ret.data().second.at(0).value(0).toString());

  ui->pushButton_simpan->setEnabled(true);
  ui->stackedWidget->setCurrentIndex(1);
}

void HalamanPage::on_pushButton_cari_clicked() {
  if (ui->comboBox_kategori->currentIndex() == 0) {
    ui->formTableView->setfilter (
        std::make_pair(ui->comboBox_kategori->currentIndex(), ui->lineEdit_inputcari->text ()));
  }
  if (ui->comboBox_kategori->currentIndex() == 1) {
    ui->formTableView->setfilter(
        std::make_pair(1, ui->lineEdit_inputcari->text()));
  }
  if (ui->comboBox_kategori->currentIndex() == 2) {
    ui->formTableView->setfilter(
        std::make_pair(2, ui->lineEdit_inputcari->text()));
  }
  if (ui->comboBox_kategori->currentIndex() == 3) {
      Param2LikeAndEqual param;
      param.index=3;
      param.equal=ui->comboBox_kategori_nomor->currentIndex ()+1;
      param.like=ui->lineEdit_inputcari->text ();
      ui->formTableView->setfilterunik1(param);
  }
  if (ui->comboBox_kategori->currentIndex() == 4) {
    if (ui->comboBox_cari_provider->count() == 0) {
      return;
    }
    Param2VariantAndVariant param;
    param.index=4;
    param.filter1=ui->comboBox_cari_provider->currentData ();
    param.filter2=ui->comboBox_kategori_nomor->currentIndex ()+1;
    ui->formTableView->setfilterunik2 (param);
  }
}

void HalamanPage::on_comboBox_kategori_currentIndexChanged(int index) {
  if (index == 0) {
        ui->lineEdit_inputcari->setPlaceholderText (placeholdernomor);
    ui->lineEdit_inputcari->show ();
    ui->comboBox_cari_provider->hide();
    ui->comboBox_kategori_nomor->hide ();
  }
  if (index == 1) {
    ui->lineEdit_inputcari->show();
    ui->comboBox_cari_provider->hide();
     ui->comboBox_kategori_nomor->hide ();
     ui->lineEdit_inputcari->setPlaceholderText (placeholdernomor);
  }
  if (index == 2) {
    ui->lineEdit_inputcari->show();
    ui->comboBox_cari_provider->hide();
       ui->comboBox_kategori_nomor->hide ();
       ui->lineEdit_inputcari->setPlaceholderText (placeholdernomor);
  }
  if (index == 3) {
    ui->lineEdit_inputcari->show();
    ui->comboBox_cari_provider->hide();
     ui->comboBox_kategori_nomor->show ();
     ui->lineEdit_inputcari->setPlaceholderText (placeholdercluster);
  }
  if (index == 4) {
    ui->comboBox_cari_provider->clear();
     ui->comboBox_kategori_nomor->show ();
    ui->lineEdit_inputcari->hide();
    utilproduk::sqlmodelwork wk1;
    QString filter = "select id,nama from provider_nomor";
    auto ret = wk1.selectnolmtReady(*mdb, filter, true);
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
      ui->comboBox_cari_provider->addItem(
          ret.data().second.at(x).value(1).toString(),
          ret.data().second.at(x).value(0));
    }
    ui->comboBox_cari_provider->show();
  }
}

void HalamanPage::on_pushButton_cancel_clicked() {
  ui->lineEdit_inputnomor->clear();
  ui->lineEdit_kota->clear();
  ui->lineEdit_inputnomor->setFocus();
  ui->stackedWidget->setCurrentIndex(0);
}

void HalamanPage::on_pushButton_simpan_clicked() {
  auto returndetermined = nomorutil::NumberParserType::determineNumber(
      ui->lineEdit_inputnomor->text().toStdString());
  if (!returndetermined.isnumbervalid) {
    QString errormsg = "";
    errormsg += QString("Nomor %1, ")
                    .arg(returndetermined.nomor_string_diprocess.c_str());
    if (returndetermined.km == nomorutil::KategoriNomor::NOMOR_UNKNOWN) {
      errormsg += "kategori nomor tidak diketahui. ";
    } else {
      if (returndetermined.km == nomorutil::KategoriNomor::NOMOR_10_DIGIT) {
        errormsg += "kategori nomor tersebut 10 digit. ";
      }
      if (returndetermined.km == nomorutil::KategoriNomor::NOMOR_ACAK) {
        errormsg += "kategori nomor tersebut nomor acak. ";
      }
      if (returndetermined.km == nomorutil::KategoriNomor::NOMOR_CANTIK) {
        errormsg += "kategori nomor tersebut nomor cantik. ";
      }
    }
    if (returndetermined.pn == nomorutil::ProviderNomor::PROVIDER_3) {
      errormsg += " Provider nomor tersebut 3. ";
    }
    if (returndetermined.pn == nomorutil::ProviderNomor::PROVIDER_AXIS) {
      errormsg += " Provider nomor tersebut axis. ";
    }
    if (returndetermined.pn == nomorutil::ProviderNomor::PROVIDER_INDOSAT) {
      errormsg += " Provider nomor tersebut indosat.";
    }
    if (returndetermined.pn == nomorutil::ProviderNomor::PROVIDER_TELKOMSEL) {
      errormsg += " Provider nomor tersebut telkomsel.";
    }
    if (returndetermined.pn == nomorutil::ProviderNomor::PROVIDER_XL) {
      errormsg += " Provider nomor tersebut xl.";
    }
    error::dialog::emitdialogerror(
        this, tr("Galat"),
        tr("Validasi nomor tidak berhasil, klik selengkapnya untuk melihat "
           "diagnosa analisis kami."),
        errormsg);
    return;
  }
  if (modeadd_or_edit) {
    auto current = QDateTime::currentDateTime();
    auto fmt = current.toString("ddMMyyyyhhmmsszzz");
    QVariantList row;
    row << ui->lineEdit_inputnomor->text();
    row << fmt;
    row << static_cast<int>(returndetermined.pn);
    row << ui->lineEdit_kota->text();
    row << static_cast<int>(returndetermined.km);
    row << QDate::currentDate();
    utilproduk::sqlmodelwork del;
    QString insert =
        "insert or replace into nomor_telp "
        "(`nomor`,`nama`,`id_provider`,`cluster`,`id_kategori`,`tanggal_dibuat`"
        ") values "
        "(:0,:1,:2,:3,:4,:5);";
    std::pair<bool, QString> ret = del.editReady(*mdb, insert, row);
    if (!ret.first) {
      error::dialog::emitdialogerror(
          this, tr("Galat"),
          tr("Terjadi galat ketika proses menambah entri data "
             ". "
             "Klik selengkapnya untuk pesan lebih lanjut."),
          ret.second);
      return;
    }
    ui->lineEdit_inputnomor->clear();
    ui->lineEdit_kota->clear();
    ui->formTableView->thisrefresh();
    ui->stackedWidget->setCurrentIndex(0);
  } else {
    // mode edit
    QVariantList row;
    row << ui->lineEdit_inputnomor->text();
    row << static_cast<int>(returndetermined.pn);
    row << ui->lineEdit_kota->text();
    row << static_cast<int>(returndetermined.km);
    row << last_id_edit;
    utilproduk::sqlmodelwork del;
    QString insert =
        "update nomor_telp set "
        "nomor=:0, id_provider=:1,cluster=:2,id_kategori=:3  where id =:4";
    std::pair<bool, QString> ret = del.editReady(*mdb, insert, row);
    if (!ret.first) {
      error::dialog::emitdialogerror(
          this, tr("Galat"),
          tr("Terjadi galat ketika proses menambah entri data "
             ". "
             "Klik selengkapnya untuk pesan lebih lanjut."),
          ret.second);
      return;
    }
    ui->lineEdit_inputnomor->clear();
    ui->lineEdit_kota->clear();
    ui->formTableView->thisrefresh();
    ui->stackedWidget->setCurrentIndex(0);
  }
}

void HalamanPage::on_lineEdit_inputnomor_textChanged(const QString &arg1) {
  if (ui->lineEdit_kota->text().isEmpty() || arg1.isEmpty()) {
    ui->pushButton_simpan->setEnabled(false);
  } else {
    ui->pushButton_simpan->setEnabled(true);
  }
}

void HalamanPage::on_lineEdit_kota_textChanged(const QString &arg1) {
  if (ui->lineEdit_inputnomor->text().isEmpty() || arg1.isEmpty()) {
    ui->pushButton_simpan->setEnabled(false);
  } else {
    ui->pushButton_simpan->setEnabled(true);
  }
}

void HalamanPage::setDataViewExportNameKategori() {
    qDebug()<<Q_FUNC_INFO<<"CALLED";
  switch (ui->comboBox_cari_provider->currentIndex()) {
    case 0:
      ui->formTableView->setNamedKategoriExport("Kategori_10_digit");
      break;
    case 1:
      ui->formTableView->setNamedKategoriExport("Kategori_nomor_cantik");
      break;
    case 2:
      ui->formTableView->setNamedKategoriExport("Kategori_nomor_cantik");
      break;
    case 3:
      ui->formTableView->setNamedKategoriExport("Kategori_nomor_cantik");
      break;
    case 4:
      ui->formTableView->setNamedKategoriExport(
          QString("Kategori_provider_%1")
              .arg(ui->comboBox_kategori->itemText(ui->comboBox_kategori->currentIndex ())));
      break;
    default:
      break;
  }
}
