#include <asklogin.h>
#include <halamanpage.h>
#include <halamanprovider.h>
#include <importnomorcsv.h>
#include <mainwindow.h>
#include <memory.h>
#include <pengaturanhowmany.h>
#include <prosesbar.h>
#include <resetdatanomor.h>
#include <tentangaplikasi.h>
#include <uiutil.h>
#include <QCloseEvent>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMenu>
#include <QMessageBox>
#include "tabwidgetutama.h"
#include "ui_menu.h"
#include <QFileDialog>
#include <utilproduk.h>
#include <nomornonduplicate.h>
#include <importnomornonduplicate.h>
#include <hapusdatanomordimaster.h>

form::MainMenu::MainMenu() : QMainWindow(), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  spawnlogindialog();
  isloginok = false;
  bar_for_reset = new prosesbar(this);

  bar_for_reset->setmsg(
      tr("Sedang Melakukan reset data nomor..").toStdString().c_str());
  bar_for_reset->setWindowFlags(bar_for_reset->windowFlags() &
                                ~Qt::WindowCloseButtonHint);
  menudb = new QMenu(this);
  act_buatdb =
      menudb->addAction(tr("Buat Database"), this, SLOT(buatdatabasebaru()),QKeySequence("CTRL+N"));
  act_opendb =
      menudb->addAction(tr("Buka Database lain"), this, SLOT(opendatabase()),QKeySequence("CTRL+O"));
  ui->toolButton_database->setMenu(menudb);
  mdb["dbbackend"] = "QSQLITE";
  
  menuimportdata=new QMenu(this);
  menuimportdata->addAction(tr("Import Data Master(Format Nomor,Cluster)"),this,SLOT(importdataNomorAndCluster()));
  menuimportdata->addAction(tr("Import Data Non-duplicate(Format Nomor saja)"),this,SLOT(importdataNomorSaja()));
  menuimportdata->addAction(tr("Hapus Data Master via import data csv (Format Nomor saja)"),this,SLOT(importHapusDuplicateNomorDataMaster()));
  ui->toolButton_upload_ms_access->setMenu (menuimportdata);
  this->hidealltooldb ();
  connect(ui->tabWidget_utama,SIGNAL(emitOpentabDataMaster()),SLOT(setuphalamanPage()));
  connect(ui->tabWidget_utama,SIGNAL(emitOpentabDataNonDuplicate()),SLOT(on_toolButton_non_duplicate_clicked()));
  resetdatamenu=new QMenu(this);
  resetdatamenu->addAction(tr("Reset Data Master (Nomor, Cluster)"),this,SLOT(resetdatamastersaja()));
  resetdatamenu->addAction(tr("Reset Data Non-duplicate(Nomor Saja)"),this,SLOT(resetdataNonduplicatenomor()));
  ui->toolButton_reset_data_nomor->setMenu (resetdatamenu);
  
}
void form::MainMenu::closeEvent(QCloseEvent *evt) { evt->accept(); }

void form::MainMenu::setuphalamanPage() {
  HalamanPage *hal = new HalamanPage;
  connect(hal, &HalamanPage::asktoinputdata, [this]() {

  });
  hal->setdbconfig(&mdb);
  hal->setProperty ("window",1);
  ui->tabWidget_utama->addTab(hal, tr("Data Master"));
}

form::MainMenu::~MainMenu() {
  bar_for_reset->deleteLater();
  menudb->deleteLater();
  menuimportdata->deleteLater ();
  resetdatamenu->deleteLater ();
  delete ui;
}

void form::MainMenu::on_actionKeluar_triggered() { close(); }

void form::MainMenu::spawnlogindialog() {}

void form::MainMenu::recieveloginInfo(const QByteArray &cnf) {
  isloginok = true;
  this->show();
  QJsonDocument doc = QJsonDocument::fromJson(cnf);
  if (doc.isNull()) {
    return;
  }
  mdb["dbbackend"] = "QMYSQL";
  mdb["hostname"] = doc.object()["hostname"].toString();
  mdb["dbname"] = doc.object()["dbname"].toString();
  mdb["username"] = doc.object()["username"].toString();
  mdb["password"] = doc.object()["password"].toString();
  mdb["port"] = doc.object()["port"].toInt();
  setuphalamanPage();
}

void form::MainMenu::on_toolButton_data_master_clicked() { setuphalamanPage(); }

void form::MainMenu::on_toolButton_data_provider_clicked() {
  HalamanProvider *hal = new HalamanProvider;
  hal->setdbconfig(&mdb);
  ui->tabWidget_utama->addTab(hal, tr("Data Provider"));
}

void form::MainMenu::importdataNomorAndCluster() {
  ImportNomorCSV *im = new ImportNomorCSV(this);
  connect(im, &ImportNomorCSV::accepted, [this]() {
    this->ui->tabWidget_utama->remembercloseandOpenTab ();
  });
  im->setAttribute(Qt::WA_DeleteOnClose);
  im->setconfigdb(mdb);
  im->exec();
}

void form::MainMenu::on_toolButton_pengaturan_clicked() {
  PengaturanHowMany *m = new PengaturanHowMany(this);
  connect(m, &PengaturanHowMany::accepted, [this]() {
    this->ui->tabWidget_utama->remembercloseandOpenTab ();
  });
  m->setAttribute(Qt::WA_DeleteOnClose);
  m->exec();
}


void form::MainMenu::buatdatabasebaru() {
  auto fname=QFileDialog::getSaveFileName (this,tr("Simpan Baru Dulu Nama Database File yang diinginkan"),".","*.telpon.db");
  if(fname.isEmpty ()){
      this->hidealltooldb ();
     isloginok=false;
         this->ui->tabWidget_utama->clear();
      return;
  }
  QFileInfo fa(fname);
  if(fa.exists ()){
      QMessageBox ask(this);
      ask.setIcon(QMessageBox::Information);
    
      ask.setInformativeText(
          tr("File database %1 sudah ada, apakah anda yakin ingin melakukan overwrite file tersebut  ?")
              .arg(fname));
      ask.setText(tr("Konfirmasi pembuatan file database baru."));
      QPushButton *ya = ask.addButton(tr("Ya"), QMessageBox::ActionRole);
      QPushButton *tidak = ask.addButton(tr("Tidak jadi"), QMessageBox::ActionRole);
      ask.setDefaultButton(tidak);
      ask.exec();
      if (ask.clickedButton() == tidak) {
        return;
      }
      if (ask.clickedButton() == ya) {
          QFile::remove (fname);
          QFile fs(fname);
          if(!fs.open (QIODevice::WriteOnly)){
              error::dialog::emitdialogerror (this,tr("Galat"),
                                              tr("File %1 tidak bisa ditulis oleh sistem app. Pastikan file database tersebut tidak sedang terbuka atau digunakan aplikasi lain.").arg (fname));
               return;
          }
          QFile fdb("://res/template.telpon.db");
          if(!fdb.open (QIODevice::ReadOnly)){
              error::dialog::emitdialogerror (this,tr("Galat"),
                                              tr("Internal error file aplikasi korup. Kami mohon untuk meminta file update app baru, jika masih masa maintenance update software. s").arg (fname));
          
               return;
          }
          while(!fdb.atEnd ()){
              fs.write (fdb.readLine ());
          }
          fdb.close ();
          fs.close ();
          mdb["dbname"]=fname;
          isloginok=true;
          this->showalltooldb ();
          this->ui->tabWidget_utama->clear();
          this->setuphalamanPage();
          return;
      }
  }else{
      QFile fs(fname);
      if(!fs.open (QIODevice::WriteOnly)){
          error::dialog::emitdialogerror (this,tr("Galat"),
                                          tr("File %1 tidak bisa ditulis oleh sistem app. Pastikan file database tersebut tidak sedang terbuka atau digunakan aplikasi lain.").arg (fname));
           return;
      }
      QFile fdb("://res/template.telpon.db");
      if(!fdb.open (QIODevice::ReadOnly)){
          error::dialog::emitdialogerror (this,tr("Galat"),
                                          tr("Internal error file aplikasi korup. Kami mohon untuk meminta file update app baru, jika masih masa maintenance update software. s").arg (fname));
      
           return;
      }
      while(!fdb.atEnd ()){
          fs.write (fdb.readLine ());
      }
      fdb.close ();
      fs.close ();
      mdb["dbname"]=fname;
      isloginok=true;
      this->showalltooldb ();
      this->ui->tabWidget_utama->clear();
      this->setuphalamanPage();
      return;
  }
}

void form::MainMenu::opendatabase() {
    auto fname=QFileDialog::getOpenFileName (this,tr("Pilih Database File Ekstensi *.telpon.db "),".","*.telpon.db");
    if(fname.isEmpty ()){
          this->hidealltooldb ();
         isloginok=false;
             this->ui->tabWidget_utama->clear();
        return;
    }
    QFile fs(fname);
    if(!fs.open (QIODevice::ReadWrite)){
        error::dialog::emitdialogerror (this,tr("Galat"),
                                        tr("File %1 tidak bisa dibaca oleh sistem app.").arg (fname));
          this->hidealltooldb ();
         isloginok=false;
             this->ui->tabWidget_utama->clear();
        return;
    }
    fs.close ();
    mdb["dbname"]=fname;
    utilproduk::sqlmodelwork wk;
    std::tuple<bool, QString, QString> ret=wk.testloginReady (mdb);
    if(std::get<0>(ret)==false){
        error::dialog::emitdialogerror (this,tr("Galat"),
                                        tr("File database %1 tidak bisa dibuka.").arg (fname));
        isloginok=false;
        this->hidealltooldb ();
            this->ui->tabWidget_utama->clear();
        return;
    }else{
        //test setiap kolom db
        if(!testkolomdb ()){
            error::dialog::emitdialogerror (this,tr("Galat")
                                            ,tr("File Database %1 tidak bisa digunakan karena terdapat komponen yang korup. Anda bisa memilih yang lain"));
            
            isloginok=false;
            this->hidealltooldb ();
            this->ui->tabWidget_utama->clear();
            return;
        }
        isloginok=true;
        this->showalltooldb ();
        this->ui->tabWidget_utama->clear();
        this->setuphalamanPage();
    }
}

void form::MainMenu::hidealltooldb()
{
    ui->groupBox_data_master->setEnabled (false);
    ui->groupBox_data_provider->setEnabled (false);
    ui->groupBox_import_csv->setEnabled (false);
    ui->groupBox_pengaturan->setEnabled (false);
    ui->groupBox_reset_nomor->setEnabled (false);
   ui->groupBox_non_duplicate->setEnabled (false);
}

void form::MainMenu::showalltooldb()
{  
    ui->groupBox_data_master->setEnabled (true);
    ui->groupBox_data_provider->setEnabled (true);
    ui->groupBox_import_csv->setEnabled (true);
    ui->groupBox_pengaturan->setEnabled (true);
    ui->groupBox_reset_nomor->setEnabled (true);
    ui->groupBox_non_duplicate->setEnabled (true);
}

bool form::MainMenu::testkolomdb()
{
    utilproduk::sqlmodelwork wk1;
    QString sql="select id,nama from kategori_nomor; ";
    auto ret=wk1.selectnolmtReady (mdb,sql);
    if(!ret.data ().first){
        return false;
    }
    if(ret.data ().second.size ()!=4){
        return false;
    }
    ret.avoiddeepcopy ();
    sql="select id,nama,tanggal_dibuat from provider_nomor;";
    ret=wk1.selectnolmtReady (mdb,sql);
    if(!ret.data ().first){
        return false;
    }
    ret.avoiddeepcopy ();
    sql="select id,nama,nomor,id_provider,cluster,id_kategori,tanggal_dibuat from nomor_telp where id=:0 limit :1,:2 ;";
    auto ret2=wk1.filterequalbysinglegroupReady (mdb,sql,1,qMakePair(0,1));
    if(!ret2.data ().first){
        return false;
    }
    qDebug()<<"ret2"<<ret2.message ()<<"size"<<ret2.data ().second.size ();
    sql="select id,nomor,id_provider,id_kategori,tanggal_dibuat from nomor_nonduplicate where id= :0 limit :1,:2 ; ";
    auto ret1=wk1.filterequalbysinglegroupReady (mdb,sql,1,qMakePair(0,1));
    if(!ret1.data ().first){
        qDebug()<<"dropping db nomor_nonduplicate"<<ret1.message ();
        sql="drop table if exists nomor_nonduplicate;";
        auto cr=wk1.crdelalterReady (mdb,sql);
        if(!cr.first){
            return false;
        }
        sql="CREATE TABLE `nomor_nonduplicate` ("
                "`id`	INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE,"
                "`nomor`	TEXT NOT NULL UNIQUE,"
                "`id_provider`	INTEGER NOT NULL,"
                "`id_kategori`	INTEGER NOT NULL,"
                "`tanggal_dibuat`	TEXT NOT NULL"
            ");";
         cr=wk1.crdelalterReady (mdb,sql);
        if(!cr.first){
            return false;
        }
    }
    return true; 
}

void form::MainMenu::on_toolButton_non_duplicate_clicked()
{
    NomorNonDuplicate *non=new NomorNonDuplicate(this);
    non->setProperty ("window",2);
    non->setconfigdb (&mdb);
    ui->tabWidget_utama->addTab (non,tr("Nomor non-duplicate"));
}

void form::MainMenu::importdataNomorSaja()
{
 ImportNomorNonDuplicate *non=new ImportNomorNonDuplicate(this);
 connect(non,&ImportNomorNonDuplicate::accepted,[this](){
     this->ui->tabWidget_utama->remembercloseandOpenTab (2);
 });
 non->setAttribute (Qt::WA_DeleteOnClose);
 non->setconfigdb (&mdb);
 non->exec ();
}

void form::MainMenu::resetdatamastersaja()
{
    QMessageBox ask(this);
    ask.setIcon(QMessageBox::Information);
  
    ask.setInformativeText(
        tr("Apakah anda yakin untuk melakukan Reset Hapus Data Nomer Master? "));
    ask.setText(tr("Konfirmasi reset data nomor."));
    QPushButton *ya = ask.addButton(tr("Ya"), QMessageBox::ActionRole);
    QPushButton *tidak = ask.addButton(tr("Tidak jadi"), QMessageBox::ActionRole);
    ask.setDefaultButton(tidak);
    ask.exec();
    if (ask.clickedButton() == tidak) {
      return;
    }
    if (ask.clickedButton() == ya) {
      bar_for_reset->setWindowTitle(tr("Proses berlangsung..."));
      bar_for_reset->open();
      std::pair<bool, QString> ret = ResetDataNomor::askresetDataNomor(mdb);
      bar_for_reset->accept();
      if (!ret.first) {
        error::dialog::emitdialogerror(this, tr("Galat"),
                                       tr("Terjadi galat ketika melakukan reset "
                                          "data nomor, klik selengkapnya untuk "
                                          "mengetahui penyebabnya."),
                                       ret.second);
        return;
      }
      ui->tabWidget_utama->clear ();
      this->setuphalamanPage ();
    }
}

void form::MainMenu::resetdataNonduplicatenomor()
{
    QMessageBox ask(this);
    ask.setIcon(QMessageBox::Information);
  
    ask.setInformativeText(
        tr("Apakah anda yakin untuk melakukan Reset Hapus Data Nomer Non-duplicate? "));
    ask.setText(tr("Konfirmasi reset data nomor."));
    QPushButton *ya = ask.addButton(tr("Ya"), QMessageBox::ActionRole);
    QPushButton *tidak = ask.addButton(tr("Tidak jadi"), QMessageBox::ActionRole);
    ask.setDefaultButton(tidak);
    ask.exec();
    if (ask.clickedButton() == tidak) {
      return;
    }
    if (ask.clickedButton() == ya) {
      bar_for_reset->setWindowTitle(tr("Proses berlangsung..."));
      bar_for_reset->open();
      std::pair<bool, QString> ret = ResetDataNomor::askresetDataNomorNonDuplicate (mdb);
      bar_for_reset->accept();
      if (!ret.first) {
        error::dialog::emitdialogerror(this, tr("Galat"),
                                       tr("Terjadi galat ketika melakukan reset "
                                          "data nomor, klik selengkapnya untuk "
                                          "mengetahui penyebabnya."),
                                       ret.second);
        return;
      }
      ui->tabWidget_utama->clear ();
      on_toolButton_non_duplicate_clicked();
    }    
}

void form::MainMenu::importHapusDuplicateNomorDataMaster()
{
   HapusDataNomorDiMaster *dm=new HapusDataNomorDiMaster(this);
   dm->setAttribute (Qt::WA_DeleteOnClose);
   connect(dm,&HapusDataNomorDiMaster::accepted,[this](){
       this->ui->tabWidget_utama->clear ();
       this->setuphalamanPage ();
   });
   dm->setconfigdb (mdb);
   dm->exec ();
}
