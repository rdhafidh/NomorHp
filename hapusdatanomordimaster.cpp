#include "hapusdatanomordimaster.h"
#include <hapusbyimportdatanomordimaster.h>
#include <importhapusnomoronlyparser.h>
#include <prosesbar.h>
#include <uiutil.h>
#include "ui_hapusdatanomordimaster.h"

HapusDataNomorDiMaster::HapusDataNomorDiMaster(QWidget *parent)
    : QDialog(parent), ui(new Ui::HapusDataNomorDiMaster) {
  ui->setupUi(this);
  qRegisterMetaType<std::vector<QByteArray>>("std::vector<QByteArray>");

  dm = new HapusByImportDataNomorDiMaster;
  connect(dm, SIGNAL(processDone()), SLOT(progressdone()));
  connect(dm, SIGNAL(processFailed(QString)), SLOT(progressfailed(QString)));
  connect(dm, SIGNAL(processStep(qint64)), SLOT(progresstep(qint64)));
  bar = new prosesbar(this);
  connect(bar,&prosesbar::asktocancelProsesBerlangsung,[this](){
      //hanya enable pas identifikasi saja
      this->reject ();
      
  });
  bar->setWindowFlags(bar->windowFlags() & ~Qt::WindowCloseButtonHint);

  np = new ImportHapusNomorOnlyParser;
  connect(np,SIGNAL(emitImportManyCheckSize(qint64)),SLOT(handleImportManyCheckSize(qint64)));
  connect(np,SIGNAL(emitImportCheckStep(qint64)),SLOT(handleImportCheckStep(qint64)));
  connect(np, &ImportHapusNomorOnlyParser::emitImportFailed,
          [this](const QString &msg) {
            this->bar->accept();
            error::dialog::emitdialogerror(this, tr("Galat"), msg);
            this->reject();
          });
  connect(
      np, &ImportHapusNomorOnlyParser::emitImportDone,
      [this](const std::vector<QByteArray> &data) {
        this->bar->accept();
        this->datas = data;

        if (data.size() == 0) {
          error::dialog::emitdialoginfo(
              this, tr("Informasi"),
              tr("Data yang ditemukan kosong dalam database untuk dihapus."));
          reject ();
          return;
        }
        QMessageBox ask(this);
        ask.setIcon(QMessageBox::Information);

        ask.setInformativeText(tr("Telah ditemukan sejumlah %1 data dalam "
                                  "database yang terduplikasi untuk bisa dihapus, apakah anda yakin untuk "
                                  "menghapus data-data nomor tersebut? ")
                                   .arg(data.size()));
        ask.setText(tr("Konfirmasi penghapusan data nomor."));
        QPushButton *ya = ask.addButton(tr("Ya"), QMessageBox::ActionRole);
        QPushButton *tidak =
            ask.addButton(tr("Tidak jadi"), QMessageBox::ActionRole);
        ask.setDefaultButton(tidak);
        ask.exec();
        if (ask.clickedButton() == tidak) {
          this->reject();
          return;
        }
        if (ask.clickedButton() == ya) {
          this->dorealhapus();
          return;
        }
        this->reject();
      });

  connect(this, &HapusDataNomorDiMaster::accepted, [this]() {
    this->np->selesai();
    this->np->disconnect ();
      delete this->np;
    this->dm->selesai();
    this->dm->disconnect ();
      delete this->dm;
      
  });
  connect(this, &HapusDataNomorDiMaster::rejected, [this]() {
    this->np->selesai();
    this->np->disconnect ();
      delete this->np;
    this->dm->selesai();
    this->dm->disconnect ();
      delete this->dm;
  });
}

HapusDataNomorDiMaster::~HapusDataNomorDiMaster() {
  qDebug()<<Q_FUNC_INFO;
  bar->disconnect ();
  delete bar;
  delete ui;
}

void HapusDataNomorDiMaster::setconfigdb(const dbbase::configdb &mdb) {
  config = mdb;
}

void HapusDataNomorDiMaster::on_pushButton_upload_nomor_hapus_clicked() {
  fname = QFileDialog::getOpenFileName(this, tr("Pilih File format nomor saja"),
                                       tr("."), "*.csv");
  if (fname.isEmpty()) {
    ui->pushButton_jalankan_hapus_nomor->setEnabled(false);
    ui->lineEdit_upload_nomor->clear();
    return;
  }

  QFile ftest(fname);
  if (!ftest.open(QIODevice::ReadOnly | QIODevice::Text)) {
    error::dialog::emitdialogerror(
        this, tr("Galat"), tr("File %1 tidak bisa dibuka. Pastikan anda "
                              "memiliki hak izin akses ke file tersebut.")
                               .arg(fname));
    ui->pushButton_jalankan_hapus_nomor->setEnabled(false);
    ui->lineEdit_upload_nomor->clear();
    return;
  }
  ftest.close();
  ui->pushButton_jalankan_hapus_nomor->setEnabled(true);
  ui->lineEdit_upload_nomor->setText(fname);
}

void HapusDataNomorDiMaster::on_pushButton_jalankan_hapus_nomor_clicked() {
  np->setup(fname, ui->checkBox_is_use_header_csv->isChecked(), config);
  np->doimport();
  bar->setmsg(tr("Sedang melakukan identifikasi nomor duplicate data master...")
                  .toStdString()
                  .c_str());
  bar->setWindowTitle(tr("Proses identifikasi data"));
  bar->open();
}

void HapusDataNomorDiMaster::on_pushbutton_tidak_jadi_clicked() { reject(); }

void HapusDataNomorDiMaster::dorealhapus() {
   bar->setEnableCancelTask (false);
  dm->setconfigdb(config, datas);
  dm->doprocessing();
  bar->setmsg(tr("Sedang melakukan penghapus nomor duplicate data master...")
                  .toStdString()
                  .c_str());
  bar->setWindowTitle(tr("Proses penghapusan data"));
  bar->open();
  bar->setmax(datas.size());
  bar->setprogressStep(0);
}

void HapusDataNomorDiMaster::progresstep(qint64 n) { bar->setprogressStep(n); }

void HapusDataNomorDiMaster::progressdone() {
  bar->accept();
  accept();
}

void HapusDataNomorDiMaster::progressfailed(const QString &msg) {
  bar->accept();
  error::dialog::emitdialogerror(this, tr("Galat"), msg);
  reject();
}

void HapusDataNomorDiMaster::handleImportManyCheckSize(qint64 n)
{
    bar->setmax (n);
    bar->setEnableCancelTask (true);
}

void HapusDataNomorDiMaster::handleImportCheckStep(qint64 n)
{
    std::cout<<"\nke "<<n;
    bar->setprogressStep (n);
}
